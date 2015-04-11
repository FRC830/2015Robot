#include "WPILib.h"

#include "../util/830utilities.h"
#include "../auton/auton_programs.h"
#include "MecanumDrive.h"

#include "Lifter.h"
#include "Roller.h"
#include "ToteHandler.h"

#include "robot_identifier.h"

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

	BuiltInAccelerometer * accel;

	ADXRS450Gyro * gyro;

	DigitalLED * led;

	GamepadF310 * pilot;
	GamepadF310 * copilot;

	PowerDistributionPanel * pdp;

	AutonProgram * auton_program;
	ToteOnly * tote_only;
	BinOnly * bin_only;
	BinAndTote * bin_and_tote;
	MoveForward * move_forward;
	SitStill * sit_still;

	SendableChooser * auton_chooser;

	SendableChooser * teleop_chooser;
	//to store our chosen teleop program, use a function pointer
	//this is a pointer to a function that takes nothing and returns nothing
	//so we can use it for our auton program
	typedef void (*teleop_program)();
	teleop_program current_teleop;

	LiveWindow * lw;
	DriverStation * ds;

	float last_dpad_y;

	//normalizes the angles of the gyro
	//converts all angles to be between 0 and 360 degrees
	int NormalizedAngle(float angle){
		int i_angle = (int) angle;
		if (i_angle >= 0) {
			return i_angle % 360;
		} else {
			return 360 + i_angle % 360;
		}
	}

	void RobotInit()
	{
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

		gyro = new ADXRS450Gyro();
		accel = new BuiltInAccelerometer();

		//led = new DigitalLED(LED_RED_DIO, LED_GREEN_DIO, LED_BLUE_DIO);

		SmartDashboard::init();

		pdp = new PowerDistributionPanel();

		//tote_only = new ToteOnly(lifter, roller, tote_handler, drive);
		//bin_only = new BinOnly(lifter, roller, tote_handler, drive);
		//bin_and_tote = new BinAndTote(lifter, roller, tote_handler, drive);
		//move_forward = new MoveForward(lifter, roller, tote_handler, drive);
		//sit_still = new SitStill(lifter, roller, tote_handler, drive);
		auton_chooser = new SendableChooser();

		//these guys have to be pointers
		auton_chooser->AddDefault("tote only", tote_only);
		auton_chooser->AddObject("bin only", bin_only);
		//auton_chooser->AddObject("bin and tote", bin_and_tote);
		auton_chooser->AddObject("move forward", move_forward);
		auton_chooser->AddDefault("sit still", sit_still);
		auton_program = sit_still;

		//this is horrible sketchy yes but it works
		teleop_chooser = new SendableChooser();
		teleop_chooser->AddDefault("official", (void *) &Robot::TeleopControlPeriodic);
		teleop_chooser->AddObject("test", (void *) &Robot::TeleopTestPeriodic);

		SmartDashboard::PutData("auton chooser", auton_chooser);
		SmartDashboard::PutData("teleop chooser", teleop_chooser);
		SmartDashboard::PutNumber("roller speed", 1.0); //set up smart dashboard variables we want to read from
		SmartDashboard::PutNumber("lifter speed", 1.0);


		lw = LiveWindow::GetInstance();
		ds = DriverStation::GetInstance();

		last_dpad_y = 0.0;
	}

	void DisabledInit() {
		gyro->Stop();
		tote_handler->ReturnToDefault();
	}

	void DisabledPeriodic() {
		drive->Brake();
		//update auton and teleop modes from smart dashboard

		AutonProgram * auton = (AutonProgram *) auton_chooser->GetSelected();
		if (auton) {
			auton_program = auton;
		} else {
			auton_program = sit_still;
		}
		teleop_program teleop = (teleop_program) teleop_chooser->GetSelected();
		if (teleop) {
			current_teleop = teleop;
		} else {
			current_teleop = (teleop_program) &Robot::TeleopControlPeriodic;
		}

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", lifter->AtBottom());
		SmartDashboard::PutBoolean("cal line broken", calibration_switch->Get());
	}

	void AutonomousInit()
	{
		if (auton_program) {
			auton_program->Init();
		} else {
			sit_still->Init();
		}
		//move_forward->Init();
	}

	void AutonomousPeriodic()
	{

		if (auton_program) {
			auton_program->Periodic();
		} else {
			sit_still->Periodic();
		}

		//move_forward->Periodic();
	}

	void TeleopInit()
	{
		gyro->Reset();

		if (!tote_handler->Calibrated()) {
			tote_handler->Calibrate();
		}

		//gyro->Start();
	}

	void TeleopPeriodic() {
		if (current_teleop) {
			current_teleop();
		}
		//TeleopTestPeriodic();
	}

	//see controls.txt for control scheme
	void TeleopControlPeriodic()
	{
		//drive controls
		float right_y = pilot->RightY();
		if (right_y > 0.9 || right_y < -0.9) {
			drive->Brake();
		} else if (!(pilot->LeftTrigger() >= 0.3|| pilot->RightTrigger() >= 0.3)) {
			//go slow unless triggers pressed
			drive->DriveCartesian(pilot->LeftX() / 1.0, pilot->LeftY() / 1.3, pilot->RightX() / 1.9);
		} else {
			drive->DriveCartesian(pilot->LeftX() / 1.0, pilot->LeftY() / 1.0, pilot->RightX() / 1.5);
		}

		//copilot tote handling controls
		if(copilot->Button(GamepadF310::B_Button)) {
			tote_handler->GatherBin();
		} else if (copilot->Button(GamepadF310::A_Button)) {
			tote_handler->GatherTote();
		} else if (copilot->Button(GamepadF310::X_Button)) {
			tote_handler->GatherFromFeeder();
		} else if (copilot->Button(GamepadF310::Y_Button)) {
			tote_handler->GoToStep();
		} else if (copilot->RightBumper()) {
			tote_handler->PickUp();
		} else if (copilot->LeftBumper()) {
			tote_handler->RaiseTote();
		} else if (copilot->DPadX() == 1.0){
			tote_handler->Eject();
		} else if (copilot->Button(GamepadF310::START_BUTTON)) {
			tote_handler->Calibrate();
		} else if (copilot->Button(GamepadF310::BACK_BUTTON)) {
			tote_handler->GoToFloor();
		} else if (fabs(copilot->LeftY()) >= 0.3 || fabs(copilot->LeftX()) >= 0.3) {
			//cancel command if left stick wiggled
			tote_handler->ReturnToDefault();
		}

		roller->Rotate(copilot->RightX());

		float dpad_y = copilot->DPadY();
		if (dpad_y == 1.0 && last_dpad_y != 1.0) {
			tote_handler->IncreaseHeight();
		} else if (dpad_y == -1.0 && last_dpad_y != -1.0) {
			tote_handler->DecreaseHeight();
		}
		last_dpad_y = dpad_y;

		//only does anything when lifter under tote
		//tote_handler->ManualRoller(copilot->RightX(), copilot->RightY());

		if (copilot->LeftTrigger() >= 0.4 || copilot->RightTrigger() >= 0.4) {
			tote_handler->Override();
			if (copilot->DPadY() == 1.0) {
				lifter_motor->Set(-1.0);
			} else if (copilot->DPadY()) {
				lifter_motor->Set(1.0);
			} else {
				lifter_motor->Set(0.0);
			}
			if (copilot->Button(GamepadF310::A_Button)) {
				left_roller_motor->Set(0.8);
				right_roller_motor->Set(-0.8);
			} else if (copilot->Button(GamepadF310::Y_Button) || copilot->DPadX() == 1.0) {
				left_roller_motor->Set(-0.3);
				right_roller_motor->Set(0.3);
			} else if (copilot->Button(GamepadF310::BACK_BUTTON)) {
				left_roller_motor->Set(-1.0);
				right_roller_motor->Set(-1.0);
			} else if (copilot->Button(GamepadF310::START_BUTTON)) {
				left_roller_motor->Set(1.0);
				right_roller_motor->Set(1.0);
			} else {
				left_roller_motor->Set(0.0);
				right_roller_motor->Set(0.0);
			}
		}

		tote_handler->Update(); //need to call this for anything to happen

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", lifter->AtBottom());
		SmartDashboard::PutBoolean("lifter calibrated", tote_handler->Calibrated());
		SmartDashboard::PutBoolean("cal line broken", calibration_switch->Get());
		//SmartDashboard::PutNumber("Lifter Current", pdp->GetCurrent(2));
	}

	void TeleopTestPeriodic()
	{
		//in case we accidentally start a match in test
		if (copilot->Button(GamepadF310::START_BUTTON)) {
			current_teleop = (teleop_program) &Robot::TeleopControlPeriodic;
		}

		float right_y = pilot->RightY();
		if (right_y > 0.9 || right_y < -0.9) {
			drive->Brake();
		} else if (pilot->LeftBumper() || pilot->RightBumper()) {
			drive->DriveCartesian(pilot->LeftX() / 1.5, pilot->LeftY() / 1.5, pilot->RightX() / 1.5);
		} else {
			drive->DriveCartesian(pilot->LeftX(), pilot->LeftY(), pilot->RightX());
		}

		float lifter_speed = SmartDashboard::GetNumber("lifter speed");

		float dpad_x = copilot->DPadX();
		float dpad_y = copilot->DPadY();

		if(dpad_y == -1 && bottom_switch->Get()) {
			lifter_motor->Set(lifter_speed);
			SmartDashboard::PutString("lift state", "lifting down!");
		} else if(dpad_y == 1) {
			lifter_motor->Set(-lifter_speed);
			SmartDashboard::PutString("lift state", "lifting up!");
		} else {
			lifter_motor->Set(0.0);
			SmartDashboard::PutString("lift state", "lifting not!");
		}

		if (lifter->AtBottom()){
			lift_encoder->Reset();
		}

		float roller_speed = SmartDashboard::GetNumber("roller speed");
		if (dpad_x == -1) {
			left_roller_motor->Set(roller_speed);
			right_roller_motor->Set(-roller_speed);
			SmartDashboard::PutString("roller state", "rolling in!");
		} else if(dpad_x == 1) {
			left_roller_motor->Set(-roller_speed);
			right_roller_motor->Set(roller_speed);
			SmartDashboard::PutString("roller state", "rolling out!");
		} else {
			left_roller_motor->Set(0.0);
			right_roller_motor->Set(0.0);
			SmartDashboard::PutString("roller state", "rolling not!");
		}

		if (roller->ToteCaptured()){
			//led->Set(DigitalLED::kGreen);
		}else{
			enum DriverStation::Alliance alliance = ds->GetAlliance();
			if (alliance == DriverStation::kBlue){
				//led->Set(DigitalLED::kBlue);
			}
			else if (alliance == DriverStation::kRed){
				//led->Set(DigitalLED::kRed);
			}
		}

		//SmartDashboard::PutNumber("front left", pdp->GetCurrent(14));
		//SmartDashboard::PutNumber("rear left", pdp->GetCurrent(15));
		//SmartDashboard::PutNumber("front right", pdp->GetCurrent(1));
		//SmartDashboard::PutNumber("rear right", pdp->GetCurrent(0));

		//SmartDashboard::PutBoolean("user button", GetUserButton());
		//SmartDashboard::PutNumber("gyro angle", gyro->GetAngle());
		//SmartDashboard::PutNumber("gyro rate", gyro->GetRate());
		//SmartDashboard::PutNumber("offset", gyro->Offset());

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", lifter->AtBottom());
		SmartDashboard::PutBoolean("cal line broken", calibration_switch->Get());
		//SmartDashboard::PutNumber("Lifter Current", pdp->GetCurrent(2));

		//lw->Run()
	}
};

START_ROBOT_CLASS(Robot);
