//
//  system_configuration.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/19/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef system_configuration_hpp
#define system_configuration_hpp

#include <stdio.h>
#include <string>
#include "super_string.hpp"

using namespace std;

class SystemConfiguration
{
public:
    SystemConfiguration();
    ~SystemConfiguration();
    
    float   get_logic_battery_level ( );
    float   get_power_battery_level ( );
    void    get_logic_battery_SOH   ( string& mBattery );
    void    get_power_battery_SOH   ( string& mBattery );

    float   get_fraction_of_working_subsystems();
    
    void    state_your_health       ( string& mOverallHealth );
    void    whats_wrong             ( string& mFailingSubsystemsStatement );
    void    how_can_it_be_fixed     ( string& mFailingSolutionsStatement  );
    
    float   m_battery_logic_level;
    float   m_battery_power_level;
    
    SuperString  m_expected_subsystems;       //
    SuperString  m_available_subsystems;      // Use a ',' seperated list.
};



#endif /* system_configuration_hpp */
