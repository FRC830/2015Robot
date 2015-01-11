#include "WPILib.h"
#include "GamepadF310.h"

class Robot: public IterativeRobot
{
private:
	static const int MOTOR_ONE = 1;
	static const int MOTOR_TWO = 2;
	static const int MOTOR_THREE = 3;

	Victor * motor_1;
	Victor * motor_2;
	Victor * motor_3;
	BuiltInAccelerometer * accel;

	GamepadF310 * pilot;

	AxisCamera * camera;

	LiveWindow *lw;

	void RobotInit()
	{
		motor_1 = new Victor(MOTOR_ONE);
		motor_2 = new Victor(MOTOR_TWO);
		motor_3 = new Victor(MOTOR_THREE);

		pilot = new GamepadF310(0);

		accel = new BuiltInAccelerometer();

		camera = new AxisCamera("10.8.30.11");

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

		SmartDashboard::PutNumber("accel x", accel->GetX());
		SmartDashboard::PutNumber("accel y", accel->GetY());
		SmartDashboard::PutNumber("accel z", accel->GetZ());

		lw->Run();
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot);
