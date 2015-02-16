#include "WPILib.h"

#include "../util/830utilities.h"
#include "../auton/auton_programs.h"
#include "MecanumDrive.h"

#include "Lifter.h"
#include "Roller.h"
#include "ToteHandler.h"

//compile this code for the practice robot if PRACTICE_ROBOT is defined
#define PRACTICE_ROBOT

class Robot: public IterativeRobot
{
private:
	static const int ROLLER_LEFT_PWM = 6;
	static const int ROLLER_RIGHT_PWM = 8;
	static const int LIFTER_PWM = 7;

	//drivetrain
	static const int LEFT_FRONT_PWM = 0;
	static const int LEFT_REAR_PWM = 1;
	static const int RIGHT_FRONT_PWM = 2;
	static const int RIGHT_REAR_PWM = 3;

	//servo
	static const int YAW_SERVO_PWM = 4;
	static const int PITCH_SERVO_PWM = 5;

	//Roller setup
	static const int ROLLER_LINEBREAK_DIO = 0;
	Victor * left_roller_motor;
	Victor * right_roller_motor;
	Roller * roller;
	DigitalInput * roller_linebreak;

	//lifter setup
	static const int TOP_SWITCH_DIO = 1;
	static const int BOTTOM_SWITCH_DIO = 2;
	static const int ENC_A_DIO = 3;
	static const int ENC_B_DIO = 4;
	Victor * lifter_motor;
	Lifter * lifter;
	DigitalInput * bottom_switch;
	DigitalInput * top_switch;
	Encoder * lift_encoder;

	ToteHandler * tote_handler;

	MecanumDrive * drive;

	//acceleration control for drivetrain
	static constexpr float TIME_TO_MAX_SPEED = 2.0;

	//servos to control the position of the camera
	Servo * yaw_servo;
	Servo * pitch_servo;


	BuiltInAccelerometer * accel;

	ADXRS450Gyro * gyro;

	GamepadF310 * pilot;
	GamepadF310 * copilot;

	float camerax;
	float cameray;

	PowerDistributionPanel * pdp;

	AutonProgram * auton_program;
	ToteOnly * tote_only;
	BinOnly * bin_only;

	SendableChooser * auton_chooser;

	SendableChooser * teleop_chooser;
	//to store our chosen teleop program, use a function pointer
	//this is a pointer to a function that takes nothing and returns nothing
	//so we can use it for our auton program
	typedef void (*teleop_program)();
	teleop_program current_teleop;

	LiveWindow *lw;

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
		top_switch = new DigitalInput(TOP_SWITCH_DIO);
		lift_encoder = new Encoder(ENC_A_DIO, ENC_B_DIO);
		lifter = new Lifter(lifter_motor, lift_encoder, bottom_switch, top_switch);

		tote_handler = new ToteHandler(roller, lifter);

		yaw_servo = new Servo(YAW_SERVO_PWM);
		pitch_servo = new Servo(PITCH_SERVO_PWM);

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

		camerax = 90.0;
		cameray = 90.0;

		SmartDashboard::init();

		pdp = new PowerDistributionPanel();

		tote_only = new ToteOnly(lifter, roller, drive);
		bin_only = new BinOnly(lifter, roller, drive);
		auton_chooser = new SendableChooser();
		//these guys have to be pointers

		auton_chooser->AddDefault("tote only", tote_only);
		auton_chooser->AddObject("bin only", bin_only);
		auton_program = tote_only;



		//this is horrible sketchy yes but it works
		teleop_chooser = new SendableChooser();
		teleop_chooser->AddDefault("test", (void *) &Robot::TeleopTestPeriodic);
		teleop_chooser->AddObject("official", (void *) &Robot::TeleopControlPeriodic);



		SmartDashboard::PutData("auton chooser", auton_chooser);
		SmartDashboard::PutData("teleop chooser", teleop_chooser);
		SmartDashboard::PutNumber("roller speed", 0.0); //set up smart dashboard variables we want to read from
		SmartDashboard::PutNumber("lifter speed", 0.0);


		lw = LiveWindow::GetInstance();


	}

	void DisabledInit() {
		gyro->Stop();
	}

	void DisabledPeriodic() {
		drive->Brake();
		//update auton and teleop modes from smart dashboard
		auton_program = (AutonProgram *) auton_chooser->GetSelected();
		current_teleop = (teleop_program) teleop_chooser->GetSelected();

		SmartDashboard::PutNumber("encoder value", lift_encoder->Get());
		SmartDashboard::PutBoolean("tote captured", roller->ToteCaptured());
		SmartDashboard::PutBoolean("arm at bottom", bottom_switch->Get());
	}

	void AutonomousInit()
	{
		auton_program->Init();
	}

	void AutonomousPeriodic()
	{
		auton_program->Periodic();
	}

	void TeleopInit()
	{
		gyro->Reset();

		//gyro->Start();
	}

	void TeleopPeriodic() {
		current_teleop();
		//TeleopTestPeriodic();
	}

	//see controls.txt for control scheme
	void TeleopControlPeriodic()
	{
		//drive controls
		float right_y = pilot->RightY();
		if (right_y > 0.9 || right_y < -0.9) {
			drive->Brake();
		} else if (pilot->LeftBumper() || pilot->RightBumper()) {
			drive->DriveCartesian(pilot->LeftX() / 2.0, pilot->LeftY() / 2.0, pilot->RightX() / 2.0);
		} else {
			drive->DriveCartesian(pilot->LeftX(), pilot->LeftY(), pilot->RightX());
		}

		//copilot tote handling controls

		roller->SetRotation(copilot->RightX());
		if(copilot->Button(GamepadF310::B_Button)) {
			tote_handler->PickupBin();
		} else if (copilot->Button(GamepadF310::A_Button)) {
			tote_handler->PickupTote();
		} else if (copilot->Button(GamepadF310::X_Button)) {
			tote_handler->EjectToFloor();
		} else if (copilot->Button(GamepadF310::Y_Button)) {
			tote_handler->EjectToStep();
		} else if (fabs(copilot->LeftY() >= 0.5) || fabs(copilot->LeftX() >= 0.5)) {
			//cancel command if left stick wiggled
			tote_handler->Cancel();
		}

		tote_handler->Update(); //need to call this for anything to happen
	}

	void TeleopTestPeriodic()
	{
		float right_y = pilot->RightY();
		if (right_y > 0.9 || right_y < -0.9) {
			drive->Brake();
		} else if (pilot->LeftBumper() || pilot->RightBumper()) {
			drive->DriveCartesian(pilot->LeftX() / 1.5, pilot->LeftY() / 1.5, pilot->RightX() / 1.5);
		} else {
			drive->DriveCartesian(pilot->LeftX(), pilot->LeftY(), pilot->RightX());
		}

		float lifter_speed = SmartDashboard::GetNumber("lifter speed");

		//lifter (test) code
		if(copilot->Button(GamepadF310::A_Button) && !bottom_switch->Get()){
			lifter_motor->Set(lifter_speed);
			SmartDashboard::PutString("lift state", "lifting down!");
		}else if(copilot->Button(GamepadF310::Y_Button)){
			lifter_motor->Set(-lifter_speed);
			SmartDashboard::PutString("lift state", "lifting up!");
		}else{
			lifter_motor->Set(0.0);
			SmartDashboard::PutString("lift state", "lifting not!");
		}

		float roller_speed = SmartDashboard::GetNumber("roller speed");
		float dpad_y = copilot->DPadY();
		if(dpad_y == -1 && !roller->ToteCaptured()){
			left_roller_motor->Set(roller_speed);
			SmartDashboard::PutString("roller state", "rolling in!");
		}else if(dpad_y == 1){
			left_roller_motor->Set(-roller_speed);
			SmartDashboard::PutString("roller state", "rolling out!");
		}else{
			left_roller_motor->Set(0.0);
			SmartDashboard::PutString("roller state", "rolling not!");
		}

		//move camera using DPad input
		if(pilot->DPadX()==1.0){
			camerax = camerax + 1.0;
		} else if(pilot->DPadX()==-1.0){
			camerax = camerax - 1.0;
		}
		if(pilot->DPadY()==1.0){
			cameray = cameray + 1.0;
		} else if(pilot->DPadY()==-1.0){
			cameray = cameray -1.0;
		}
		yaw_servo->SetAngle(camerax);
		pitch_servo->SetAngle(cameray);

		tote_handler->Override(); //automation ain't ready for primetime yet
		//tote_handler->Update(); //updating this also updates the lifter and the roller

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
		SmartDashboard::PutBoolean("arm at bottom", bottom_switch->Get());

		//lw->Run()
	}
};

START_ROBOT_CLASS(Robot);
