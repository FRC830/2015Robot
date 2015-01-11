#include "WPILib.h"
#include "GamepadF310.h"

class Robot: public IterativeRobot
{
private:
	static const int MOTOR_ONE = 1;
	static const int MOTOR_TWO = 2;
	static const int MOTOR_THREE = 3;

	static const int GYRO_ANALOG = 1;

	static const int ENCODER_A = 2;
	static const int ENCODER_B = 3;

	static const int ANGULAR_SENSOR_ANALOG = 4;

	static const int SOLENOID = 1;

	Victor * motor_1;
	Victor * motor_2;
	Victor * motor_3;
	BuiltInAccelerometer * accel;

	GamepadF310 * pilot;

	//SmartDashboard * smart_dash;

	LiveWindow *lw;

	void RobotInit()
	{
		motor_1 = new Victor(MOTOR_ONE);
		motor_2 = new Victor(MOTOR_TWO);
		motor_3 = new Victor(MOTOR_THREE);

		pilot = new GamepadF310(0);

		accel = new BuiltInAccelerometer();

		SmartDashboard::init();

		lw = LiveWindow::GetInstance();


	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}

	void TeleopPeriodic()
	{
		motor_1->Set(pilot->LeftY());
		motor_2->Set(pilot->RightY());

		SmartDashboard::PutNumber("DPad X", pilot->DPadX());
		SmartDashboard::PutNumber("DPad Y", pilot->DPadY());
		SmartDashboard::PutString("message", "hi!");

		lw->Run();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot);
