#include "PC_FileIO.c"

const int MAX_ANGLE = 360;

void penPosition(int pos)
{
	if(pos == 0)
		//up
	{
		motor[motorB] = 50;
  	wait1Msec(300);
  	motor[motorB] = 0;
	}
	else
	{
		motor[motorB] = -50;
  	wait1Msec(300);
  	motor[motorB] = 0;
	}
}

void calibrateGyro()
{
	SensorType[S4] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S4] = modeEV3Gyro_Calibration;
	wait1Msec(50);
	SensorMode[S4] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
}

void ConfigureAllSensors()
{
  calibrateGyro();
  SensorType[S1] = sensorEV3_Color;
	wait1Msec(50);
  SensorMode[S1] = modeEV3Color_Color;
	wait1Msec(50);
	penPosition(0);
}

bool isRed()
{
	if(SensorValue(S1) == (int)colorRed)
	{
		return true;
	}
	return false;
}

void driveDist(int distance)
{
	const int ENC_LIMIT = distance*360/(2*PI*2.75);
	nMotorEncoder[motorA] = 0;


	if(distance > 0)
	{
			motor[motorA] = motor[motorD] = 30;
			while(nMotorEncoder[motorA] < ENC_LIMIT)
			{}
	}

	else
	{
			motor[motorA] = motor[motorD] = -30;
			while(nMotorEncoder[motorA] > ENC_LIMIT)
			{}
	}

	motor[motorA] = motor[motorD] = 0;
}

void turnAngle(int angle)
{
	resetGyro(S4);
	motor[motorA] = 20;
	motor[motorD] = -20;
	while(getGyroDegrees(S4) < angle)
	{}
	motor[motorA] = motor[motorD] = 0;
}

void squareLineup()
{
  penPosition(0);
  driveDist(14);
  turnAngle(90);
  driveDist(-15);
}

void triangleLineup()
{
	penPosition(0);
  driveDist(14);
  turnAngle(120);
  driveDist(-16.5);

}

void drawSquare(int sideLength, bool &success)
{
  if(isRed())
  {
    displayString(13, "The robot went outside");
		displayString(14, "the boundaries!");
    success = false;
  }
  else
  {
    for(int count = 0; count < 4; count++)
    {
      penPosition(1);
      nMotorEncoder[motorA] = 0;
      float distance = sideLength * (360/(2 * PI * 2.75));
      motor[motorA] = motor[motorD] = 20;

      while(SensorValue(S1) != (int)colorRed && nMotorEncoder[motorA] <= distance)
      {}
      motor[motorA] = motor[motorD] = 0;

      if(SensorValue(S1) == (int)colorRed)
        break;

      if(count < 3)
      {
        squareLineup();
      }
    }
  }


}


void drawTriangle(int sideLength, bool &success)
{
	if(isRed())
  {
    displayString(13, "The robot went outside");
		displayString(14, "the boundaries!");
    success = false;
  }
  else
  {
  	for(int count = 0; count < 3; count++)
	  {
		  penPosition(1);
		  nMotorEncoder[motorA] = 0;
		  float distance = sideLength * (360/(2 * PI * 2.75));
			motor[motorA] = motor[motorD] = 20;

			while(SensorValue(S1) != (int)colorRed && nMotorEncoder[motorA] <= distance)
			{}
		  motor[motorA] = motor[motorD] = 0;

		  if(SensorValue(S1) == (int)colorRed)
		  {
		  	break;
		  }

		  triangleLineup();
	  }
	  penPosition(0);
  }

}


void circleDraw(bool &success)
{
	if(isRed())
	{
		displayString(13, "The robot went outside");
		displayString(14, "the boundaries!");
		success = false;
	}
	else
	{
		resetGyro(S4);
		penPosition(1);
		motor[motorA] = 20;
		motor[motorD] = -20;
		while(SensorValue(S1) != (int)colorRed && getGyroDegrees(S4) < MAX_ANGLE)
		{}
		motor[motorA] = motor[motorD] = 0;
		penPosition(0);
	}
}

void outputDisplay(float time, int count, bool success)
{
	if(success)
			displayString(1, "%d shapes were drawn.", count);
	displayString(2, "The shapes attempted");
	displayString(3, "to be drawn:");
	displayString(12, "Time taken: %f seconds", time/1000.0);
}

void successPrint(bool success, string shape, int &count)
{
	if(success)
	{
		count++;
		displayString(3+count, "%s", shape);
	}
}

task main()
{
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}
	ConfigureAllSensors();
	TFileHandle fin;
	string shape = "";
	float sidelength = 0;
	int count = 0;
	bool success = true;

	bool fileOkay = openReadPC(fin,"demo1.txt");
	if (!fileOkay)
	{
		displayString(5,"Error!");
		wait1Msec(5000);
	}
	clearTimer(T1);
	while(readTextPC(fin, shape))
	{
		if(shape == "square" || shape == "triangle")
		{
			readFloatPC(fin, sidelength);
			if(shape == "square")
			{
				drawSquare(sidelength, success);
				successPrint(success, shape, count);
			}

			else
			{
				drawTriangle(sidelength, success);
				successPrint(success, shape, count);
			}
		}

		else
		{
				circleDraw(success);
				successPrint(success, shape, count);
		}
	}

  outputDisplay(time1[T1], count, success);

  while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}
}
