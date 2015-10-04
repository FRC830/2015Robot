#include "WPILib.h"

#include "../util/830utilities.h"
#include "../auton/auton_programs.h"
#include "Lifter.h"
#include "Lifter.h"
#include "MecanumDrive.h"
#include "robot_identifier.h"
#include "Roller.h"
#include "ToteHandler.h"

struct IMU {
	int16_t roll, pitch, yaw;
};

int16_t getSPIInt(SPI *s) {
	static uint8_t cmd = 0;
	uint8_t byte0, byte1;
	s->Transaction(&cmd, &byte0, 1);
	s->Transaction(&cmd, &byte1, 1);
	return (byte1 << 8) + byte0;
}

IMU readIMU(SPI *s) {
	static uint8_t cmd = 0, out;
	//s->SetChipSelectActiveHigh();
	s->Transaction(&cmd, &out, 1);
	IMU imu;
	imu.roll = getSPIInt(s);
	imu.pitch = getSPIInt(s);
	imu.yaw = getSPIInt(s);
	for (int i = 0; i < 6; ++i)
		getSPIInt(s);
	//s->SetChipSelectActiveLow();
	return imu;
}

class Robot: public IterativeRobot
{
private:
	//drivetrain
	static const int LEFT_FRONT_PWM = 0;
	static const int LEFT_REAR_PWM = 1;
	static const int RIGHT_FRONT_PWM = 2;
	static const int RIGHT_REAR_PWM = 3;

	static const int ROLLER_LEFT_PWM = 5;
	static const int ROLLER_RIGHT_PWM = 6;
	static const int LIFTER_PWM = 8;

	static const int ROLLER_LINEBREAK_DIO = 0;
	static const int CALIBRATION_SWITCH_DIO = 1;
	static const int BOTTOM_SWITCH_DIO = 2;
	static const int ENC_A_DIO = 3;
	static const int ENC_B_DIO = 4;

	static const int LED_RED_DIO = 7;
	static const int LED_GREEN_DIO = 8;
	static const int LED_BLUE_DIO = 9;

	static const int GYRO_ANALOG_PIN = 0;

	//Roller setup
	Victor * left_roller_motor;
	Victor * right_roller_motor;
	Roller * roller;
	DigitalInput * roller_linebreak;

	//lifter setup
	Victor * lifter_motor;
	Lifter * lifter;
	DigitalInput * bottom_switch;
	DigitalInput * calibration_switch;
	Encoder * lift_encoder;

	ToteHandler * tote_handler;

	MecanumDrive * drive;

	//acceleration control for drivetrain
	static constexpr float TIME_TO_MAX_SPEED = 2.0;

	DigitalLED * led;

	GamepadF310 * pilot;
	GamepadF310 * copilot;

	PowerDistributionPanel * pdp;

	AutonProgram * auton_program;
	AutonProgram * default_auton; //keep a default auton program in case there's trouble with the smart dashboard
	SendableChooser * auton_chooser;

	SPI *compass;
	Gyro * gyro;
	bool picked_up;
	float initial_angle;
	Timer * auton_timer;

	bool test_mode; //whether we're in test mode, with manual controls only

	DriverStation * ds;

	float last_dpad_y;

	// Teleop variables:
	float desired_angle;

	void RobotInit()
	{
		compass = new SPI(SPI::kOnboardCS0);
		compass->SetClockRate(100000);

		gyro  = new Gyro(GYRO_ANALOG_PIN);
		auton_timer = new Timer;

		left_roller_motor = new Victor(ROLLER_LEFT_PWM);
		right_roller_motor = new Victor(ROLLER_RIGHT_PWM);
		roller_linebreak = new DigitalInput(ROLLER_LINEBREAK_DIO);
		roller = new Roller(left_roller_motor, right_roller_motor, roller_linebreak);

		lifter_motor = new Victor(LIFTER_PWM);
		bottom_switch = new DigitalInput(BOTTOM_SWITCH_DIO);
		calibration_switch = new DigitalInput(CALIBRATION_SWITCH_DIO);
		lift_encoder = new Encoder(ENC_A_DIO, ENC_B_DIO);
		lifter = new Lifter(lifter_motor, lift_encoder, bottom_switch, calibration_switch);

		tote_handler = new ToteHandler(roller, lifter);

#ifdef PRACTICE_ROBOT
		RobotDrive * robot_drive = new RobotDrive(
						new Talon(LEFT_FRONT_PWM), new Talon(LEFT_REAR_PWM),
						new Talon(RIGHT_FRONT_PWM), new Talon(RIGHT_REAR_PWM));
#else
		RobotDrive * robot_drive = new RobotDrive(
				new VictorSP(LEFT_FRONT_PWM), new VictorSP(LEFT_REAR_PWM),
				new VictorSP(RIGHT_FRONT_PWM), new VictorSP(RIGHT_REAR_PWM));
#endif

		robot_drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
		robot_drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, false  );
		robot_drive->SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		robot_drive->SetInvertedMotor(RobotDrive::kRearRightMotor, true);
		drive = new MecanumDrive(robot_drive, TIME_TO_MAX_SPEED);

		pilot = new GamepadF310(0);
		copilot = new GamepadF310(1);

		led = new DigitalLED(LED_RED_DIO, LED_GREEN_DIO, LED_BLUE_DIO);

		SmartDashboard::init();

		pdp = new PowerDistributionPanel();

		auton_chooser = new SendableChooser();
		default_auton = new SitStill(lifter, roller, tote_handler, drive);
		auton_chooser->AddDefault("sit still", default_auton);
		auton_chooser->AddDefault("tote only", new ToteOnly(lifter, roller, tote_handler, drive));
		auton_chooser->AddObject("bin only", new BinOnly(lifter, roller, tote_handler, drive));
		auton_chooser->AddObject("move forward", new MoveForward(lifter, roller, tote_handler, drive));
		auton_program = default_auton;

		test_mode = false;
		SmartDashboard::PutBoolean("test mode", false);

		SmartDashboard::PutData("auton chooser", auton_chooser);
		SmartDashboard::PutNumber("roller speed", 1.0); //set up smart dashboard variables we want to read from
		SmartDashboard::PutNumber("lifter speed", 1.0);

		ds = DriverStation::GetInstance();

		last_dpad_y = 0.0;
		desired_angle = 0.0;
	}

	void DisabledInit() {
		tote_handler->ReturnToDefault(); //this is so the tote handler isn't left trying to do anything that will start up again when we re-enable
	}

	void DisabledPeriodic() {
		//update auton and teleop modes from smart dashboard
		AutonProgram * auton = (AutonProgram *) auton_chooser->GetSelected();
		//if the smart dashboard isn't open, auton_chooser will return NULL
		if (auton) {
			auton_program = auton;
		} else {
			auton_program = default_auton;
		}

		test_mode = SmartDashboard::GetBoolean("test mode");

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", lifter->AtBottom());
		SmartDashboard::PutBoolean("cal line broken", calibration_switch->Get());
	}

	void AutonomousInit()
	{
		//make extra sure that auton_program is non-null before we run it
		/* Competition code
		if (auton_program) {
			auton_program->Init();
		} else {
			default_auton->Init();
		}
		*/
		if (!tote_handler->IsCalibrated()) {
			tote_handler->Calibrate();
		}
		picked_up = false;
		initial_angle = 0;
		auton_timer->Reset();
		auton_timer->Start();

		gyro->Reset();
	}

	void AutonomousPeriodic()
	{
		/*competition code
		if (auton_program) {
			auton_program->Periodic();
		} else {
			default_auton->Periodic();
		}
		*/
//		float current_angle = gyro->GetAngle();
//		float rotatePower = 0.5;
//
//
//		if(abs(current_angle - initial_angle - 90) < 10) {
//			rotatePower = 0.4;
//		}
//
//
//		//pick up a tote and turn 90 degrees to the right
//		if (!picked_up){
//			if (!roller->ToteCaptured()){
//				tote_handler->GatherTote();
//			} else {
//				tote_handler->PickUp();
//				auton_timer->Start();
//				if (auton_timer->Get() > 4.0){
//					picked_up = true;
//					initial_angle = gyro->GetAngle();
//					auton_timer->Stop();
//				}
//			}
//
//		} else if ((current_angle - initial_angle) < 89){
//			drive->DriveCartesian(0.0, 0.0, rotatePower);
//		} else if ((current_angle - initial_angle) > 91){
//			drive->DriveCartesian(0.0, 0.0, -rotatePower);
//		} else {
//			drive->DriveCartesian(0.0, 0.0, 0.0);
//		}
//		tote_handler->Update();

//		float rotatePower = 1.0;
//		drive->DriveCartesian(0.0, 0.0, rotatePower);

//		float speed = 0.5;
//

		float target_angle = 0;
		float y_speed = 0.5;
		float time = auton_timer->Get();
		float speed = 0.0;
		float angle = gyro->GetAngle();

		if (time < 4.0) {
			tote_handler->GatherTote();
		}
		if (time > 2.0) {
			y_speed = 0.0;
		}

		if (time > 4.0 && time < 6) {
			tote_handler->PickUpTote();
		}
		if (time > 6) {
			target_angle  = 180;
		}

		if (abs(angle - 180) < 1) {
			tote_handler->Eject();
			if(time > 15){
				auton_timer->Reset();
				gyro->Reset();
			}
		}

		SmartDashboard::PutNumber("angle", angle);
		float error = 1;
		if (angle > (target_angle + error)){
			speed = -0.5;

		}
		else if (angle < (target_angle - error)){
			speed = 0.5;
		}
		SmartDashboard::PutNumber("speed", speed);
		drive->DriveCartesian(0.0, y_speed, speed);

		tote_handler->Update();

	}

	void TeleopInit()
	{
		if (!tote_handler->IsCalibrated()) {
			tote_handler->Calibrate();
		}
		gyro->Reset();
	}

	//see controls.txt for control scheme explanation
	void TeleopPeriodic()
	{

		float angle = gyro->GetAngle();
		bool updating = false;
		SmartDashboard::PutNumber("Gyro angle:", angle);
		SmartDashboard::PutNumber("left y:", pilot->LeftY());
		SmartDashboard::PutNumber("left x:", pilot->LeftX());
		SmartDashboard::PutNumber("right x:", pilot->RightX());
		float rotation = pilot->RightX();
		if (fabs (pilot->RightX()) > 0.05 || (fabs (pilot->LeftX()) < 0.1 && fabs (pilot->LeftY()) < 0.05)){
			desired_angle = angle;
			updating = true;
		}
		else{
			if (desired_angle > angle){
				rotation = 0.3;
			}
			else if (desired_angle < angle){
				rotation = -0.3;
			}
		}
		SmartDashboard::PutNumber("desired angle:", desired_angle);
		SmartDashboard::PutBoolean("updating", updating);

		//drive controls
		float right_y = pilot->RightY();
		if (right_y > 0.9 || right_y < -0.9) {
			drive->Brake();

		} else if (!(pilot->LeftTrigger() >= 0.3|| pilot->RightTrigger() >= 0.3)) {
			//go slow unless triggers pressed
			drive->DriveCartesian(pilot->LeftX() / 1.0, pilot->LeftY() / 1.3, rotation / 1.9);
		} else {
			drive->DriveCartesian(pilot->LeftX() / 1.0, pilot->LeftY() / 1.0, rotation / 1.5);
		}



		//use manual controls if a trigger is pressed or if we're in test mode
		if (copilot->LeftTrigger() >= 0.4 || copilot->RightTrigger() >= 0.4 || test_mode) {
			tote_handler->Override();
			if (copilot->DPadY() == 1.0) {
				lifter_motor->Set(-1.0);
			} else if (copilot->DPadY()) {
				lifter_motor->Set(1.0);
			} else {
				lifter_motor->Set(0.0);
			}
			if (copilot->ButtonState(F310Buttons::Y)) {
				left_roller_motor->Set(0.8);
				right_roller_motor->Set(-0.8);
			} else if (copilot->ButtonState(F310Buttons::Y) || copilot->DPadX() == 1.0) {
				left_roller_motor->Set(-0.3);
				right_roller_motor->Set(0.3);
			} else if (copilot->ButtonState(F310Buttons::Back)) {
				left_roller_motor->Set(-1.0);
				right_roller_motor->Set(-1.0);
			} else if (copilot->ButtonState(F310Buttons::Start)) {
				left_roller_motor->Set(1.0);
				right_roller_motor->Set(1.0);
			} else {
				left_roller_motor->Set(0.0);
				right_roller_motor->Set(0.0);
			}

			if (copilot->ButtonState(F310Buttons::X)) {
				test_mode = false; //escape in case we accidentally start a match in test mode
				SmartDashboard::PutBoolean("test mode", false);
				tote_handler->Calibrate(); //make sure we're calibrated (would happen anyways since X calibrates in normal controls)
			}
		} else {
			//normal copilot controls
			if(copilot->ButtonState(F310Buttons::B)) {
				tote_handler->GatherBin();
			} else if (copilot->ButtonState(F310Buttons::A)) {
				tote_handler->GatherTote();
			} else if (copilot->ButtonState(F310Buttons::X)) {
				tote_handler->Calibrate();
			} else if (copilot->ButtonState(F310Buttons::Y)) {
				tote_handler->GoToStep();
			} else if (copilot->ButtonState(F310Buttons::RightBumper) || copilot->ButtonState(F310Buttons::LeftBumper)) {
				tote_handler->PickUp();
			} else if (copilot->ButtonState(F310Buttons::Back)){
				tote_handler->Eject();
			} else if (copilot->ButtonState(F310Buttons::Start)){
				tote_handler->RollIn();
			} else if (fabs(copilot->LeftY()) >= 0.3 || fabs(copilot->LeftX()) >= 0.3) {
				//cancel command if left stick wiggled
				tote_handler->ReturnToDefault();
			}

			roller->Rotate(copilot->RightX());

			//we check the previous value so holding the dpad down does not cause the lifter to go up by more than one height
			//(otherwise each tap of the dpad is pretty much guaranteed to be mulitply-counted)
			float dpad_y = copilot->DPadY();
			if (dpad_y == 1.0 && last_dpad_y != 1.0) {
				tote_handler->IncreaseHeight();
			} else if (dpad_y == -1.0 && last_dpad_y != -1.0) {
				tote_handler->DecreaseHeight();
			}
			last_dpad_y = dpad_y;
		}

		tote_handler->Update(); //need to call this for anything to happen

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", lifter->AtBottom());
		SmartDashboard::PutBoolean("lifter calibrated", tote_handler->IsCalibrated());
		SmartDashboard::PutBoolean("cal line broken", calibration_switch->Get());

//		int16_t angle;
//		compass->Read(true, (uint8_t*)&angle, sizeof(angle));
//		SmartDashboard::PutNumber("compass", angle);
		IMU imu = readIMU(compass);
#define display(v)		SmartDashboard::PutNumber(#v, imu.v)
		display(roll); display(pitch); display(yaw);
		//SmartDashboard::PutNumber("Lifter Current", pdp->GetCurrent(2));
	}
};

START_ROBOT_CLASS(Robot);
