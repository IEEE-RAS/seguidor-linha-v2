// File:          seglinV2_ctrl.cpp
// Date:
// Description:
// Author:
// Modifications:

#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/LED.hpp>

#define MAX_SPEED 10
#define REDUCTION_FACTOR 0.25

#define MIN_THRESHOLD 15
#define MAX_THRESHOLD 70

using namespace webots;

bool readSensorBinaryState(DistanceSensor *ds)
{
    double reading = ds->getValue();
    if ((MIN_THRESHOLD < reading) && (reading < MAX_THRESHOLD))
    {
        return true;
    }

    return false;
}

void stateLED(LED *led, bool oldState, bool state)
{
    if (state != oldState)
        led->set(state);
}

void updateStates(DistanceSensor *ds[], int nds, bool (&oldState)[], bool (&state)[], int ns)
{

    for (int i = 0; i < ns; i++)
    {
        oldState[i] = state[i];
    }

    for (int d = 0; d < nds; d++)
    {
        state[d] = readSensorBinaryState(ds[d]);
    }
}

void curvaAoCentro(DistanceSensor *ds, DistanceSensor *dc, LED *led, Motor *ME, Motor *MD, void (*funcao)(Motor *, Motor *))
{
    double reading = ds->getValue();

    if ((MIN_THRESHOLD < reading) && (reading < MAX_THRESHOLD))
    {
        funcao(ME, MD);
        led->set(1);
        printf("curva");
    }
}

void curvaAoCentro(bool oldState, bool state, Motor *ME, Motor *MD, void (*funcao)(Motor *, Motor *))
{
    if (state != oldState)
    {
        if (state)
            funcao(ME, MD);
    }
}

void turnLeft(Motor *ME, Motor *MD)
{
    printf("Left\n");
    ME->setVelocity(MAX_SPEED * REDUCTION_FACTOR);
    MD->setVelocity(MAX_SPEED);
}

void turnRight(Motor *ME, Motor *MD)
{
    printf("Right\n");
    MD->setVelocity(MAX_SPEED * REDUCTION_FACTOR);
    ME->setVelocity(MAX_SPEED);
}

void stop(Motor *ME, Motor *MD)
{
    ME->setVelocity(0);
    MD->setVelocity(0);
}

void forward(Motor *ME, Motor *MD)
{
    printf("forward\n");
    ME->setVelocity(MAX_SPEED);
    MD->setVelocity(MAX_SPEED);
}

int main(int argc, char **argv)
{
    // create the Robot instance.
    Robot *robot = new Robot();

    // get the time step of the current world.
    int timeStep = (int)robot->getBasicTimeStep();

    Motor *md = robot->getMotor("md");
    Motor *me = robot->getMotor("me");

    md->setPosition(INFINITY);
    md->setVelocity(MAX_SPEED);
    me->setPosition(INFINITY);
    me->setVelocity(MAX_SPEED);

    DistanceSensor *dsc = robot->getDistanceSensor("dsc");
    dsc->enable(timeStep);
    DistanceSensor *dsei = robot->getDistanceSensor("dsei");
    dsei->enable(timeStep);
    DistanceSensor *dsee = robot->getDistanceSensor("dsee");
    dsee->enable(timeStep);
    DistanceSensor *dsdi = robot->getDistanceSensor("dsdi");
    dsdi->enable(timeStep);
    DistanceSensor *dsde = robot->getDistanceSensor("dsde");
    dsde->enable(timeStep);

    DistanceSensor *dsarr[5] = {dsee, dsei, dsc, dsdi, dsde};

    LED *sled1 = robot->getLED("sled1");
    sled1->set(0);
    LED *sled0 = robot->getLED("sled0");
    sled0->set(0);
    LED *sled2 = robot->getLED("sled2");
    sled2->set(0);
    LED *sled3 = robot->getLED("sled3");
    sled2->set(0);
    LED *sled4 = robot->getLED("sled4");
    sled2->set(0);

    LED *led[5] = {sled3, sled2, sled0, sled1, sled4};

    bool state[5] = {0, 0, 0, 0, 0}, oldState[5];

    double startT = 0, finishT = 0;

    int stateOut = 0, stateOutOld = 0, detourCount = 0;
    bool takeDetour = 0;

    while (robot->step(timeStep) != -1)
    {

        stateOut = 0;

        updateStates(dsarr, 5, oldState, state, 5);

        stateOut = stateOut | (state[1] << 2);
        stateOut = stateOut | (state[2] << 1);
        stateOut = stateOut | state[3];

        for (int s = 0; s < 5; s++)
        {
            stateLED(led[s], oldState[s], state[s]);
        }

        if (state[0] != oldState[0])
            if (state[0] && state[4])
            {
                printf("code1\n");
                // takeDetour = !takeDetour;
                detourCount++;
                printf("detourCount %d\n", detourCount);
            }

        if (detourCount == 2)
            takeDetour = !takeDetour;

        if (stateOut != stateOutOld)
        {
            printf("%d\n", stateOut);
            stateOutOld = stateOut;
            switch (stateOut)
            {
            case 2:
                forward(me, md);
                break;
            case 1:
            case 3:
                if (!takeDetour)
                    turnRight(me, md);
                break;
            case 6:
            case 4:
                if (!takeDetour)
                    turnLeft(me, md);
                break;
            case 7:
                stop(me, md);
            default:
                break;
            }
        }
    };

    delete robot;
    return 0;
}
