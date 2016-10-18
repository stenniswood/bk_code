//
//  parameter.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/19/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef parameter_hpp
#define parameter_hpp

#include <stdio.h>
#include <regex>
#include "super_string.hpp"
#include "nlp_sentence.hpp"


enum eParameterType {
    FLOAT,
    INTEGER,
    BOOLEAN,
    STRING             /* user does not wish to answer the question. */
};

/*  This class parses a number (value) and a unit and optionally a name.
    Use to parse the dimensions of a box, cuboid, circle, wall, etc.
 
    This is one value:  could be a length (such as for a wall),
        "10 foot long"
        "12 feet tall"
        etc.
 */
class Parameter
{
public:
    Parameter();
    void                set_default_and_unit            ( float  mDefault,     string mDefaultUnit   );
    void                set_default_and_requested_units ( string mDefaultUnit, string mRequestedUnit );
    void                set_permitted_units             ( string mNewUnit );
    void                set_std_length_units            ( );
    void                set_metric_length_units         ( );
    void                set_std_or_metric_length_units  ( );

    string              get_specified_unit();
    string              get_requested_unit();
    
    virtual void        construct_regex ();
    bool                is_type_valid   ( );
    bool                is_match();

    void                file_results    ();
    bool                parse           ( SuperString& mSentence );    // mWordIndex is the starting word.
    bool                parse_unit      ( SuperString& mSentence );    // mWordIndex is the starting word.
    bool                parse_value     ( SuperString& mSentence, int& mWordIndex );
    void                convert_unit    ( string mNewUnit    );     // if user specified inches and program calls for feet.
    void                print_results   ();
    
    string              m_regexpression;
    std::smatch         m_matches;
    
    string              m_name;
    string              m_name_found;
    
    enum eParameterType m_type;
    SuperString         m_default_unit;         // such as "feet" for a wall.  If not specified in user text.
    string              m_requested_unit;       // The unit the value will be converted to (for the end result value)
    /* Note: default may be different than the requested.  For program that wants Liters and in US people most often cite gallons */
    
    
    SuperString         m_permitted_units;      // "(inch|inches|centimeters|meteres|feet)"
    string              m_unit_specified;

    // Value as String and Parsed:
    double              m_default_value;
    string                m_value;
    double              m_f_value;
    int                 m_i_value;
    bool                m_b_value;
};


class LengthByWidth : public Parameter
{
public:
    LengthByWidth();
    ~LengthByWidth();
    
    virtual void    construct_regex();
    void    set_name();
};



#endif /* parameter_hpp */
