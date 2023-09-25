#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../FanController.h"

/**
 * use this command to run unit test
 * 1.g++ -std=c++17 -o test testFanController.cpp ../FanController.cpp -lgtest -lgtest_main 
 * -lgmock   -pthread
 * 
 * 2. run ./test
 */
class MockVoltageSensorInterface : public VoltageSensorInterface
{
public:
    MOCK_METHOD(float, getVoltage, (), (override));
};

class MockPwmOutputInterface : public PwmOutputInterface
{
public:
    MOCK_METHOD(void, setOutputDuty, (float), (override));
};

class MockGpioOutputInterface : public GpioOutputInterface
{
public:
    MOCK_METHOD(void, setOutput, (bool), (override));
};

class Test_FanController : public ::testing::Test
{
protected:
    void TearDown() override
    {
        delete m_sensor_power_enable;
        delete m_fan_relay_enable;
    }
    void SetUp() override
    {
        m_temp_sensor_raw = std::make_unique<MockVoltageSensorInterface>();
        m_fan_output_raw = std::make_unique<MockPwmOutputInterface>();
        m_sensor_power_enable = new MockGpioOutputInterface();
        m_fan_relay_enable = new MockGpioOutputInterface();

        m_controller = std::make_unique<FanController>(*m_temp_sensor_raw,
                                                       *m_sensor_power_enable,
                                                       *m_fan_relay_enable,
                                                       *m_fan_output_raw);
    }
    std::unique_ptr<MockVoltageSensorInterface> m_temp_sensor_raw;
    std::unique_ptr<MockPwmOutputInterface> m_fan_output_raw;
    MockGpioOutputInterface *m_sensor_power_enable;
    MockGpioOutputInterface *m_fan_relay_enable;
    std::unique_ptr<FanController> m_controller;
};

TEST_F(Test_FanController, Test1)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(61))
        .WillOnce(::testing::Return(61));
    
    EXPECT_CALL(*m_fan_relay_enable, setOutput(true)).Times(1);

    // PMW = (t-50) / 2, which is 5.5
    EXPECT_CALL(*m_fan_output_raw, setOutputDuty(5.5)).Times(1);

    m_controller->init();
    m_controller->loop();

}

TEST_F(Test_FanController, Test2)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(102))
        .WillOnce(::testing::Return(102));
    
    EXPECT_CALL(*m_fan_relay_enable, setOutput(true)).Times(1);

    // PMW = (t-98) * 2 + 20, which is (102-98) *2 + 20 = 28
    EXPECT_CALL(*m_fan_output_raw, setOutputDuty(28.0)).Times(1);

    m_controller->init();
    m_controller->loop();

}

TEST_F(Test_FanController, Test3)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(130))
        .WillOnce(::testing::Return(130));
    
    EXPECT_CALL(*m_fan_relay_enable, setOutput(true)).Times(1);

    // PMW = 90.0
    EXPECT_CALL(*m_fan_output_raw, setOutputDuty(90.0)).Times(1);

    m_controller->init();
    m_controller->loop();

}

TEST_F(Test_FanController, Test4)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(141))
        .WillOnce(::testing::Return(141));
    
    EXPECT_CALL(*m_fan_relay_enable, setOutput(true)).Times(1);

    // PMW = 100.0
    EXPECT_CALL(*m_fan_output_raw, setOutputDuty(100.0)).Times(1);

    m_controller->init();
    m_controller->loop();

}

TEST_F(Test_FanController, Test5)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(49))
        .WillOnce(::testing::Return(49));
    
    EXPECT_CALL(*m_fan_relay_enable, setOutput(false)).Times(1);

    m_controller->init();
    m_controller->loop();
}

TEST_F(Test_FanController, Test6)
{
    EXPECT_CALL(*m_sensor_power_enable, setOutput(true)).Times(1).WillOnce(::testing::Return());
    EXPECT_CALL(*m_temp_sensor_raw, getVoltage()).Times(2)
        .WillOnce(::testing::Return(55))
        .WillOnce(::testing::Return(55));
    
    // TODO: clarification, if temperature is 55 then nothing happens for now.
    // Is this the expected behavior?
    EXPECT_CALL(*m_fan_relay_enable, setOutput(false)).Times(0);

    m_controller->init();
    m_controller->loop();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

