#include "WPILib.h"
#include "GamepadF310.h"

class Robot: public IterativeRobot
{
private:
	static const int MOTOR_ONE = 1;
	static const int MOTOR_TWO = 2;
	static const int MOTOR_THREE = 3;
	static const int GYRO_ANALOG = 0;

	//drivetrain
	static const int LEFT_FRONT_PWM = 6;
	static const int RIGHT_FRONT_PWM = 7;
	static const int LEFT_REAR_PWM = 8;
	static const int RIGHT_REAR_PWM = 9;

	//servo
	static const int YAW_SERVO_PWM = 5;
	static const int PITCH_SERVO_PWM = 6;

	Victor * motor_1;
	Victor * motor_2;
	Victor * motor_3;

	Talon * left_front;
	Talon * right_front;
	Talon * left_rear;
	Talon * right_rear;

	Servo * yaw_servo;
	Servo * pitch_servo;

	RobotDrive * drive;

	BuiltInAccelerometer * accel;
	Gyro * gyro;

	GamepadF310 * pilot;

	AxisCamera * camera;

	SendableChooser * auton_chooser;

	typedef void (*auton_program)();
	auton_program auton;

	LiveWindow *lw;

	//normalizes the angles of the gyro
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
		motor_1 = new Victor(MOTOR_ONE);
		motor_2 = new Victor(MOTOR_TWO);
		motor_3 = new Victor(MOTOR_THREE);

		left_front = new Talon(LEFT_FRONT_PWM);
		right_front = new Talon(RIGHT_FRONT_PWM);
		left_rear = new Talon(LEFT_REAR_PWM);
		right_rear = new Talon(RIGHT_REAR_PWM);

		yaw_servo = new Servo(YAW_SERVO_PWM);
		pitch_servo = new Servo(PITCH_SERVO_PWM);

		drive = new RobotDrive(left_front, right_front, left_rear, right_rear);

		pilot = new GamepadF310(0);

		gyro = new Gyro(GYRO_ANALOG);
		accel = new BuiltInAccelerometer();

		camera = new AxisCamera("10.8.30.11");

		SmartDashboard::init();

		//create selector object
		auton_chooser = new SendableChooser();
		//default option
		auton_chooser->AddDefault("motor 1",  (void *) &Robot::Autonomous1);
		//add option
		auton_chooser->AddObject("motor 2", (void *) &Robot::Autonomous2);
		//display on smartdashboard
		SmartDashboard::PutData("auton chooser", auton_chooser);


		lw = LiveWindow::GetInstance();


	}

	void AutonomousInit()
	{
		auton = (auton_program) auton_chooser->GetSelected();
	}

	void AutonomousPeriodic()
	{
		auton();
	}

	void Autonomous1(){
		SmartDashboard::PutString("auton", "running auton 1");
	}

	void Autonomous2() {
		SmartDashboard::PutString("auton", "running auton 2");
	}

	void TeleopInit()
	{
		yaw_servo->SetAngle(90.0);
		pitch_servo->SetAngle(90.0);
	}

	void TeleopPeriodic()
	{
		//motor_1->Set(pilot->LeftY());
		//motor_2->Set(pilot->RightY());

		float angle = gyro->GetAngle();

		//display on the SmartDashboard
		SmartDashboard::PutNumber("gyro", NormalizedAngle(angle));
		SmartDashboard::PutNumber("accel x", accel->GetX());
		SmartDashboard::PutNumber("accel y", accel->GetY());
		SmartDashboard::PutNumber("accel z", accel->GetZ());

		//mecanum test code
		float forward = pilot->LeftY();
		float strafe = pilot->LeftX();
		float rotation = pilot->RightX();

		//
		SmartDashboard::PutNumber("forward", forward);
		SmartDashboard::PutNumber("strafe", strafe);
		SmartDashboard::PutNumber("rotation", rotation);

		drive->MecanumDrive_Cartesian(forward, strafe, rotation);

		if(pilot->DPadX()==1.0){
			yaw_servo->SetAngle(yaw_servo->GetAngle()+0.1);
		} else if(pilot->DPadX()==-1.0){
			yaw_servo->SetAngle(yaw_servo->GetAngle()-0.1);
		}
		if(pilot->DPadY()==1.0){
			pitch_servo->SetAngle(pitch_servo->GetAngle()+0.1);
		} else if(pilot->DPadY()==-1.0){
			pitch_servo->SetAngle(pitch_servo->GetAngle()-0.1);
		}

		lw->Run();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot);
