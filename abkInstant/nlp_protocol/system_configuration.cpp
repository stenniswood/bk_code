//
//  system_configuration.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/19/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include "system_configuration.hpp"

const float THRESH_HOLD_1 = 0.10;
const float THRESH_HOLD_2 = 0.50;
const float THRESH_HOLD_3 = 0.75;
const float THRESH_HOLD_4 = 1.00;

const float BATTERY_THRESH_HOLD_1 = 0.90;
const float BATTERY_THRESH_HOLD_2 = 0.50;
const float BATTERY_THRESH_HOLD_3 = 0.25;


SystemConfiguration::SystemConfiguration()
{
}
SystemConfiguration::~SystemConfiguration()
{
}

float SystemConfiguration::get_logic_battery_level ( )
{
    return 0.80;
}
float SystemConfiguration::get_power_battery_level ( )
{
    return 0.80;
}
void  SystemConfiguration::get_logic_battery_SOH   ( string& mBatteryHealth )
{
    float percent  = get_logic_battery_level() * 100;
    char tmp[20];
    sprintf(tmp, "%4.0f", percent);
    mBatteryHealth = "Logic batteries are ";
    mBatteryHealth += tmp;
    mBatteryHealth += " percent ";
}
void  SystemConfiguration::get_power_battery_SOH   ( string& mBatteryHealth )
{
    float percent  = get_power_battery_level() * 100;
    char tmp[20];
    sprintf(tmp, "%4.0f", percent);
    mBatteryHealth = "Power batteries are ";
    mBatteryHealth += tmp;
    mBatteryHealth += " percent ";
}

float SystemConfiguration::get_fraction_of_working_subsystems()
{
    return 0.9;
}

void  SystemConfiguration::state_your_health( string& mOverallHealth )
{
    float fraction = get_fraction_of_working_subsystems();
    // Highest Health :
    if (fraction>=THRESH_HOLD_4) {
        if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_1) && (m_battery_power_level>=BATTERY_THRESH_HOLD_1))
            mOverallHealth = "I am great!";
        else if ((m_battery_logic_level>BATTERY_THRESH_HOLD_2) && (m_battery_power_level>=BATTERY_THRESH_HOLD_2))
            mOverallHealth = "I am fully functional!";
        else if ((m_battery_logic_level>BATTERY_THRESH_HOLD_3) && (m_battery_power_level>=BATTERY_THRESH_HOLD_3))
            mOverallHealth = "I am fully functional, but my batteries are getting low.";
        else
            mOverallHealth = "Batteries!";
    }
    
    if (fraction>THRESH_HOLD_3) {
        if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_1) && (m_battery_power_level>=BATTERY_THRESH_HOLD_1))
            mOverallHealth = "I am okay.";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_2) && (m_battery_power_level>=BATTERY_THRESH_HOLD_2))
            mOverallHealth = "I am not real good!";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_3) && (m_battery_power_level>=BATTERY_THRESH_HOLD_3))
            mOverallHealth = "I am so so - my batteries are getting low.";
        else
            mOverallHealth = "Batteries!";

    }
    if (fraction>THRESH_HOLD_2) {
        if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_1) && (m_battery_power_level>=BATTERY_THRESH_HOLD_1))
            mOverallHealth = "I am sick.";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_2) && (m_battery_power_level>=BATTERY_THRESH_HOLD_2))
            mOverallHealth = "I am sick and weak batteries.";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_3) && (m_battery_power_level>=BATTERY_THRESH_HOLD_3))
            mOverallHealth = "I am sick, and my batteries are very low.";
        else
            mOverallHealth = "Batteries!";
        
    }
    // Lowest Health :
    if (fraction>THRESH_HOLD_1) {
        
        if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_1) && (m_battery_power_level>=BATTERY_THRESH_HOLD_1))
            mOverallHealth = "I am just barely alive.";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_2) && (m_battery_power_level>=BATTERY_THRESH_HOLD_2))
            mOverallHealth = "I am just barely alive and weak batteries.";
        else if ((m_battery_logic_level>=BATTERY_THRESH_HOLD_3) && (m_battery_power_level>=BATTERY_THRESH_HOLD_3))
            mOverallHealth = "I am just barely alive and my batteries are about to give out.";
        else
            mOverallHealth = "Batteries!";
    } else
        mOverallHealth = "Dead, please bury me.";

}

void  SystemConfiguration::whats_wrong( string& mFailingSubsystemsStatement )
{
    if (m_battery_power_level < BATTERY_THRESH_HOLD_3)
        get_power_battery_SOH(mFailingSubsystemsStatement);
    if (m_battery_logic_level < BATTERY_THRESH_HOLD_3)
       get_logic_battery_SOH(mFailingSubsystemsStatement);
    // "5 of 8 subsystems are not working"
    // My [left|right] leg is broken.
    // My [left|right] arm is broken.
    
}


void  SystemConfiguration::how_can_it_be_fixed     ( string& mFailingSolutionsStatement  )
{
    
}

