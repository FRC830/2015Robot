#include "WPILib.h"

#include "../util/830utilities.h"
#include "MecanumDrive.h"

#include "Lifter.h"
#include "Roller.h"

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

	SendableChooser * auton_chooser;

	//to store our chosen auton program, use a function pointer
	//this is a pointer to a function that takes nothing and returns nothing
	//so we can use it for our auton program
	typedef void (*auton_program)();
	auton_program auton;

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

		yaw_servo = new Servo(YAW_SERVO_PWM);
		pitch_servo = new Servo(PITCH_SERVO_PWM);

		RobotDrive * robot_drive = new RobotDrive(
				new VictorSP(LEFT_FRONT_PWM), new VictorSP(LEFT_REAR_PWM),
				new VictorSP(RIGHT_FRONT_PWM), new VictorSP(RIGHT_REAR_PWM));

		robot_drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
		robot_drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		robot_drive->SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
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

		auton_chooser = new SendableChooser();

		//yes this is really ugly
		//we're making it accept the function pointer, which apparently doesn't count as void *
		//it doesn't matter what terrible things we do to the type
		//the chooser holds it, and we take it back out again
		//we have to cast the output from the chooser anyways
		auton_chooser->AddDefault("auton 1",  (void *) &Robot::Autonomous1);
		auton_chooser->AddObject("auton 2", (void *) &Robot::Autonomous2);

		SmartDashboard::PutData("auton chooser", auton_chooser);


		lw = LiveWindow::GetInstance();


	}

	void AutonomousInit()
	{
		//read the auton program from the SmartDashboard
		//cast it back to an auton_program (a function pointer)
		auton = (auton_program) auton_chooser->GetSelected();
	}

	void AutonomousPeriodic()
	{
		//use the pointer to the chosen auton function
		//make sure it's not null though, that would be bad
		if (auton) {
			auton();
		}
	}

	//a couple auton functions to test the chooser
	void Autonomous1(){
		SmartDashboard::PutString("auton", "running auton 1");
	}

	void Autonomous2() {
		SmartDashboard::PutString("auton", "running auton 2");
	}

	void TeleopInit()
	{
		gyro->Reset();

		ADXRS450Gyro::DataTest();
	}

	void TeleopPeriodic()
	{
		gyro->Update();
		lifter->Update();
		roller->Update();

		//Roller code
		if(copilot->Button(GamepadF310::A_Button)){
			roller->RollIn();
			SmartDashboard::PutString("Roll state", "Rolling In!");
		}else if(copilot->Button(GamepadF310::Y_Button)){
			roller->RollOut();
			SmartDashboard::PutString("Roll state", "Rolling Out!");
		}else{
			roller->Stop();
			SmartDashboard::PutString("Roll state", "Rolling Not!");
		}

		//Tote/bin lifting
		if (copilot->RightTrigger() || copilot->LeftTrigger()){
			lifter->MoveToPosition(Lifter::kFloor);
			SmartDashboard::PutString("Lift State", "kFloor");
		}else if(copilot->RightBumper() || copilot->LeftBumper()){
			if (lifter->AtPosition(Lifter::kTote1)){
				lifter->MoveToPosition(Lifter::kTote2);
				SmartDashboard::PutString("Lift State", "kTote2");
			} else {
				lifter->MoveToPosition(Lifter::kTote1);
				SmartDashboard::PutString("Lift State", "kTote1");
			}
		}


		//display on the SmartDashboard
		SmartDashboard::PutNumber("accel x", accel->GetX());
		SmartDashboard::PutNumber("accel y", accel->GetY());
		SmartDashboard::PutNumber("accel z", accel->GetZ());

		//mecanum test code
		float forward = pilot->LeftY(); //slow down for now because it's really fast
		float strafe = pilot->LeftX();
		float rotation = pilot->RightX();


		SmartDashboard::PutNumber("forward", forward);
		SmartDashboard::PutNumber("strafe", strafe);
		SmartDashboard::PutNumber("rotation", rotation);

		drive->DriveCartesian(forward, strafe, rotation);
		//drive->TestAll();


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

		SmartDashboard::PutNumber("front left", pdp->GetCurrent(14));
		SmartDashboard::PutNumber("rear left", pdp->GetCurrent(15));
		SmartDashboard::PutNumber("front right", pdp->GetCurrent(1));
		SmartDashboard::PutNumber("rear right", pdp->GetCurrent(0));

		SmartDashboard::PutBoolean("user button", GetUserButton());
		SmartDashboard::PutNumber("gyro angle", gyro->GetAngle());
		SmartDashboard::PutNumber("gyro rate", gyro->GetRate());
		SmartDashboard::PutNumber("offset", gyro->Offset());
		lw->Run();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot);
