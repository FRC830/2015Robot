#include "WPILib.h"

#include "../util/830utilities.h"
#include "MecanumDrive.h"

class Robot: public IterativeRobot
{
private:
	static const int ROLLER_PWM = 6;
	static const int GRIPPER_PWM = 7;
	static const int LIFTER_PWM = 8;
	static const int SHOVE_PWM = 9;

	//drivetrain
	static const int LEFT_FRONT_PWM = 0;
	static const int LEFT_REAR_PWM = 1;
	static const int RIGHT_FRONT_PWM = 2;
	static const int RIGHT_REAR_PWM = 3;

	//servo
	static const int YAW_SERVO_PWM = 4;
	static const int PITCH_SERVO_PWM = 5;

	Talon * rollers;
	Talon * gripper;
	Talon * lifter;
	Talon * shover;

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
		rollers = new Talon(ROLLER_PWM);
		gripper = new Talon(GRIPPER_PWM);
		lifter = new Talon(LIFTER_PWM);
		shover = new Talon(SHOVE_PWM);


		yaw_servo = new Servo(YAW_SERVO_PWM);
		pitch_servo = new Servo(PITCH_SERVO_PWM);

		RobotDrive * robot_drive = new RobotDrive(
				new VictorSP(LEFT_FRONT_PWM), new VictorSP(LEFT_REAR_PWM),
				new VictorSP(RIGHT_FRONT_PWM), new VictorSP(RIGHT_REAR_PWM));
		robot_drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
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
	}

	void TeleopPeriodic()
	{
		gyro->Update();
		rollers->Set(copilot->LeftY());
		gripper->Set(copilot->RightX());

		//Shove bins out
		if (copilot->Button(GamepadF310::Y_Button)){
			shover->Set(1.0);//shove out
		}else if (copilot->Button(GamepadF310::A_Button)){
			shover->Set(-1.0);//retract shove mechanism
		}else{
			shover->Set(0.0);
		}

		//Tote/bin lifting
		if (copilot->RightTrigger() || copilot->LeftTrigger()){
			lifter->Set(-1.0);//Move lifter down
		}else if(copilot->RightBumper() || copilot->LeftBumper()){
			lifter->Set(1.0);//move lifter up
		}else{
			lifter->Set(0.0);
		}


		//display on the SmartDashboard
		SmartDashboard::PutNumber("accel x", accel->GetX());
		SmartDashboard::PutNumber("accel y", accel->GetY());
		SmartDashboard::PutNumber("accel z", accel->GetZ());

		//mecanum test code
		float forward = pilot->LeftY();
		float strafe = pilot->LeftX();
		float rotation = pilot->RightX();


		SmartDashboard::PutNumber("forward", forward);
		SmartDashboard::PutNumber("strafe", strafe);
		SmartDashboard::PutNumber("rotation", rotation);

		drive->MecanumDriveCartesian(forward, strafe, rotation);

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

		SmartDashboard::PutNumber("pdp voltage (V)", pdp->GetVoltage());
		SmartDashboard::PutNumber("motor 1 current (A)", pdp->GetCurrent(3));
		SmartDashboard::PutNumber("motor 2 current (A)", pdp->GetCurrent(2));
		SmartDashboard::PutNumber("pdp temp (C)", pdp->GetTemperature());

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
