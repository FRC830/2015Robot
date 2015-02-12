#include "WPILib.h"

#include "../util/830utilities.h"
#include "MecanumDrive.h"

#include "Lifter.h"
#include "Roller.h"
#include "ToteHandler.h"
#include "AutonHandler.h"

//compile this code for the practice robot
#define PRACTICE_ROBOT

class Robot: public IterativeRobot
{
private:
	static const int ROLLER_PWM = 6;
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
	Victor * roller_motor;
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

	AutonHandler * auton_handler;

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
		roller_motor = new Victor(ROLLER_PWM);
		roller_linebreak = new DigitalInput(ROLLER_LINEBREAK_DIO);
		roller = new Roller(roller_motor, roller_linebreak);

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

		auton_handler = new AutonHandler(tote_handler, drive);
		auton_chooser = new SendableChooser();
		//these guys have to be pointers

		auton_chooser->AddDefault("tote only", (void *) AutonHandler::TOTE);
		auton_chooser->AddObject("bin only", (void *) AutonHandler::BIN);
		auton_chooser->AddObject("tote and bin", (void *) AutonHandler::TOTE_AND_BIN);
		auton_chooser->AddObject("3 tote stack", (void *) AutonHandler::THREE_TOTE_STACK);


		/*
		//this is horrible sketchy yes but it works
		teleop_chooser = new SendableChooser();
		teleop_chooser->AddDefault("test", (void *) &Robot::TeleopTestPeriodic);
		teleop_chooser->AddObject("official", (void *) &Robot::TeleopControlPeriodic);
		*/


		SmartDashboard::PutData("auton chooser", auton_chooser);
		SmartDashboard::PutNumber("roller speed", 0.0); //set up smart dashboard variables we want to read from
		SmartDashboard::PutNumber("lifter speed", 0.0);


		lw = LiveWindow::GetInstance();


	}

	void DisabledInit() {
		gyro->Stop();
	}

	void DisabledPeriodic() {
		drive->Brake();
		auton_handler->SetMode((const char *) auton_chooser->GetSelected());
		//current_teleop = (teleop_program) teleop_chooser->GetSelected();
	}

	void AutonomousInit()
	{
	}

	void AutonomousPeriodic()
	{
		auton_handler->Update();
	}

	void TeleopInit()
	{
		gyro->Reset();

		//gyro->Start();
	}

	void TeleopPeriodic() {
		//current_teleop();
		TeleopTestPeriodic();
	}

	//see controls.txt for control scheme
	void TeleopControlPeriodic()
	{
		if (pilot->LeftBumper() || pilot->RightBumper()) {
			drive->Brake();
		} else if (pilot->LeftTrigger() >= 0.2 || pilot->RightTrigger() >= 0.2) {
			//"scope" when triggers pressed
			drive->DriveCartesian(pilot->LeftX() / 2.0, pilot->LeftY() / 2.0, pilot->RightX() / 2.0);
		} else {
			drive->DriveCartesian(pilot->LeftX(), pilot->LeftY(), pilot->RightX());
		}

		if(copilot->Button(GamepadF310::B_Button)) {
			tote_handler->BinPickup();
		} else if (copilot->Button(GamepadF310::A_Button)) {
			tote_handler->TotePickup();
		} else if (copilot->Button(GamepadF310::Y_Button)) {
			tote_handler->Eject();
		} else if (copilot->Button(GamepadF310::X_Button) || fabs(copilot->LeftY() >= 0.5) || fabs(copilot->LeftX() >= 0.5)) {
			tote_handler->Cancel();
		}



		tote_handler->Update(); //need to call this for anything to happen
	}

	void TeleopTestPeriodic()
	{
		tote_handler->Update();
		tote_handler->Override(); //automation ain't ready for primetime yet

		/*
		float dpad_y = copilot->DPadY();
		if (dpad_y > 0.0 && lifter_speed <= 0.9){
			lifter_speed += 0.1;
		} else if (dpad_y < 0.0 && lifter_speed >= 0.1) {
			lifter_speed -= 0.1;
		}
		*/
		float lifter_speed = SmartDashboard::GetNumber("lifter speed");

		//lifter (test) code
		if(copilot->Button(GamepadF310::A_Button)){
			//roller->RollIn();
			lifter_motor->Set(lifter_speed);
			SmartDashboard::PutString("lift state", "lifting up!");
		}else if(copilot->Button(GamepadF310::Y_Button)){
			//roller->RollOut();
			lifter_motor->Set(-lifter_speed);
			SmartDashboard::PutString("lift state", "lifting down!");
		}else{
			//roller->Stop();
			lifter_motor->Set(0.0);
			SmartDashboard::PutString("lift state", "lifting not!");
		}

		float roller_speed = SmartDashboard::GetNumber("roller speed");
		float dpad_y = copilot->DPadY();
		if(dpad_y == -1){
			roller_motor->Set(roller_speed);
			SmartDashboard::PutString("roller state", "rolling in!");
		}else if(dpad_y == 1){
			roller_motor->Set(-roller_speed);
			SmartDashboard::PutString("roller state", "rolling out!");
		}else{
			roller_motor->Set(0.0);
			SmartDashboard::PutString("roller state", "rolling not!");
		}

		//Tote/bin lifting

		/*
		if (copilot->RightTrigger() || copilot->LeftTrigger()) {
			lifter->MoveToPosition(Lifter::kFloor);
			SmartDashboard::PutString("Lift State", "kFloor");
		}else if(copilot->RightBumper() || copilot->LeftBumper()){
			if (lifter->AtPosition(Lifter::kTote)){
				lifter->MoveToPosition(Lifter::kBin);
				SmartDashboard::PutString("Lift State", "kTote2");
			} else {
				lifter->MoveToPosition(Lifter::kTote);
				SmartDashboard::PutString("Lift State", "kTote1");
			}
		}
		*/

		if (pilot->LeftBumper() || pilot->RightBumper()) {
			drive->Brake();
		} else if (pilot->LeftTrigger() >= 0.2 || pilot->RightTrigger() >= 0.2) {
			//"scope" when triggers pressed
			drive->DriveCartesian(pilot->LeftX() / 2.0, pilot->LeftY() / 2.0, pilot->RightX() / 2.0);
		} else {
			drive->DriveCartesian(pilot->LeftX(), pilot->LeftY(), pilot->RightX());
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

		//lw->Run();
	}
};

START_ROBOT_CLASS(Robot);
