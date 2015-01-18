#include "WPILib.h"

#include "../util/830utilities.h"
#include "MecanumDrive.h"

class Robot: public IterativeRobot
{
private:
	static const int MOTOR_ONE = 3;
	static const int MOTOR_TWO = 2;
	static const int MOTOR_THREE = 6;
	static const int GYRO_ANALOG = 0;

	//drivetrain
	static const int LEFT_FRONT_PWM = 8;
	static const int RIGHT_FRONT_PWM = 1;
	static const int LEFT_REAR_PWM = 9;
	static const int RIGHT_REAR_PWM = 0;

	//servo
	static const int YAW_SERVO_PWM = 4;
	static const int PITCH_SERVO_PWM = 5;

	VictorSP * motor_1;
	VictorSP * motor_2;
	VictorSP * motor_3;


	MecanumDrive * drive;

	//acceleration control for drivetrain
	static constexpr float TIME_TO_MAX_SPEED = 2.0;

	//servos to control the position of the camera
	Servo * yaw_servo;
	Servo * pitch_servo;


	BuiltInAccelerometer * accel;
	Gyro * gyro;
	ADXRS450Gyro * gyro2;

	GamepadF310 * pilot;

	AxisCamera * camera;
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
		motor_1 = new VictorSP(MOTOR_ONE);
		motor_2 = new VictorSP(MOTOR_TWO);
		motor_3 = new VictorSP(MOTOR_THREE);

		yaw_servo = new Servo(YAW_SERVO_PWM);
		pitch_servo = new Servo(PITCH_SERVO_PWM);

		drive = new MecanumDrive(new Talon(LEFT_FRONT_PWM), new Talon(LEFT_REAR_PWM),
				new Talon(RIGHT_FRONT_PWM), new Talon(RIGHT_REAR_PWM), TIME_TO_MAX_SPEED);

		pilot = new GamepadF310(0);

		gyro = new Gyro(GYRO_ANALOG);
		gyro2 = new ADXRS450Gyro();
		accel = new BuiltInAccelerometer();

		camera = new AxisCamera("10.8.30.11");
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

	}

	void TeleopPeriodic()
	{
		gyro2->Update(); //important or gyro won't work

		motor_1->Set(pilot->LeftY());
		motor_2->Set(pilot->RightY());

		//float angle = gyro->GetAngle();

		//display on the SmartDashboard
		//SmartDashboard::PutNumber("gyro", NormalizedAngle(angle));
		SmartDashboard::PutNumber("accel x", accel->GetX());
		SmartDashboard::PutNumber("accel y", accel->GetY());
		SmartDashboard::PutNumber("accel z", accel->GetZ());

		//mecanum test code
		float forward = pilot->RightY();
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
		SmartDashboard::PutNumber("gyro output", gyro2->GetAngle());
		lw->Run();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot);
