//
//  parameter.cpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 6/19/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <iostream>
#include <map>
#include <math.h>
#include "parameter.hpp"
#include <string>
#include "super_string.hpp"


string integer_e   = "(\\d+ )";
string real_e      = "([-+]?[0-9]*\\.?[0-9]+ )";

const char* StandardLength_Units = "(inch|inhes|feet|foot|yard|yards|mile|miles )";
const char* MetricLength_Units   = "(micron|microns|micometer|micometers|um|millimeter|millimeters|mm|centimeter|centimeters|cm|meter|meters|m|kilometer|kilometers|km )";

const char* Postive_Response  = "(yes|please|okay|of course)";
const char* Negative_Response = "(no|nope|not|cancel|stop|)";


Parameter::Parameter()
{
    m_type = INTEGER;
    m_f_value = 0;
    m_i_value = 0;
    m_b_value = false;
}

void Parameter::set_default_and_unit( float  mDefault, string mDefaultUnit )
{
    m_default_value = mDefault;
    m_default_unit  = mDefaultUnit;
    m_f_value       = m_default_value;
    m_i_value       = trunc( m_f_value );
}

void Parameter::set_default_and_requested_units( string mDefaultUnit, string mRequestedUnit )
{
    m_default_unit   = mDefaultUnit  ;
    
    m_requested_unit = mRequestedUnit;
}

void Parameter::set_std_length_units( )
{
    set_permitted_units( StandardLength_Units );
}
void Parameter::set_metric_length_units( )
{
    set_permitted_units( MetricLength_Units  );
}
void  Parameter::set_std_or_metric_length_units( )
{
    string tmp = StandardLength_Units;
    tmp.erase(tmp.length()-1,1);
    tmp += '|';
    
    string tmp2 = MetricLength_Units;
    tmp2.erase(0,1);
    tmp       += tmp2;
    set_permitted_units( tmp  );
}

/******************
 Must be a selectable wordlist such as "(inch|inhes|centimeters|cm|
 ******************/
void Parameter::set_permitted_units( string mNewUnit )
{
    m_permitted_units = mNewUnit;
    m_permitted_units.split_wordlist();
}
bool Parameter::is_match()
{
    return (m_matches.size()>0) ? true:false;
}

bool Parameter::is_type_valid( )
{
    enum eParameterType m_type;
    switch( m_type )
    {
        case FLOAT   : m_f_value = stof( m_value );
            break;
        case INTEGER : m_i_value = stoi( m_value );
            break;
        case BOOLEAN :
            break;
        case STRING  :
            break;             /* user does not wish to answer the question. */
        default :
            return false;
            break;
    }
    // if it's "123.45" and the request is an integer.  then no.
    // if it's "123" and request is a boolean.  then no.
    return true;
}
string Parameter::get_specified_unit()
{
    return m_unit_specified;
    //return m_permitted_units.m_split_words[m_unit_specified].c_str();
}
string Parameter::get_requested_unit()
{
    return m_requested_unit;
    //return m_permitted_units.m_split_words[m_requested_unit].c_str();
}

string get_numeral_regexpression( enum eParameterType mType )
{
    string retval;
    switch(mType)
    {
        case FLOAT   : retval = real_e;
            break;
        case INTEGER : retval = integer_e;
            break;
        case BOOLEAN : retval = "(true|false|yes|no|okay|cancel)";
            break;
        case STRING  :
            break;             /* user does not wish to answer the question. */
        default :
            break;
    }
    return retval;
}

void Parameter::construct_regex( )
{
    m_regexpression = get_numeral_regexpression( m_type );
    //m_regexpression += " ";
    m_regexpression += m_permitted_units;
    //m_regexpression += "(feet |foot )?";
    m_regexpression += " (";
    m_regexpression += m_name;
    m_regexpression += ")";
}

bool Parameter::parse( SuperString& mSentence )
{
    bool retval = false;
    int  result = mSentence.regex_find( m_regexpression );
    m_matches  = mSentence.regex_matches;
    if (result)
        retval = true;
    file_results();
    convert_unit();
    return retval;
}

void Parameter::file_results    ()
{
    SuperString tmp;
    int    junk;
    size_t size = m_matches.size();
    for (int x=1; x<size; x++)
    {
        tmp = m_matches[x].str();
        SuperString exp = get_numeral_regexpression( m_type );
        int matches = tmp.regex_find( exp );
        if (matches)
            parse_value( tmp, junk );
        else
            parse_unit( tmp );
        matches = tmp.regex_find( m_name );
        if (matches)
            m_name_found = tmp;
    }
}

void Parameter::print_results   ()
{
    for (auto x:m_matches)
        std::cout << x << " \n";
}

bool Parameter::parse_unit( SuperString& mSentence )
{
    int result = mSentence.regex_find(m_permitted_units);
    if (result) {
        m_unit_specified = mSentence.regex_matches[0];
        return true;
    }
    return false;
}

bool Parameter::parse_value  ( SuperString& mSentence, int& mWordIndex )
{
    for (int w=0; w < mSentence.m_split_words.size(); w++)
    {
        bool result = mSentence.is_nth_word_a_number(w);
        if (result) {
            m_value = mSentence.m_split_words[w];
            if (m_type==FLOAT)
                m_f_value  = stof( mSentence.m_split_words[w] );
            else if (m_type==INTEGER)
                m_f_value  = stoi( mSentence.m_split_words[w] );
            mWordIndex = w;
            return true;
        }
    }
    return false;
}

typedef map<string,float> conversion;
map<string,conversion> converter;

void init_converter()
{
    conversion inch_c;
    inch_c["feet"] = 1/12;          // multipliers
    inch_c["yard"] = 1/36;
    inch_c["cm"]   = 2.54;
    inch_c["mm"]   = 25.4;
    inch_c["meter"]= 0.0254;
    inch_c["km"]= 0.0000254;

    conversion feet_c;
    feet_c["inch"] = 12;
    feet_c["feet"] = 1;
    feet_c["yard"] = 1/3;
    feet_c["cm"]   = 12*2.54;
    feet_c["mm"]   = 12*25.4;
    feet_c["meter"]= 12*0.0254;
    feet_c["km"]   = 12*0.0000254;

    conversion cm_c;
    cm_c["mm"]    = 10;
    cm_c["meter"] = 1/100;
    cm_c["km"]    = 1/100000;
    cm_c["inch"]  = 1/2.54;
    cm_c["feet"]  = 1/(2.54 * 12);
    cm_c["yard"]  = 1/(2.54 * 12*3);

    conversion m_c;
    cm_c["mm"]    = 1000;
    cm_c["cm"]    = 100;
    cm_c["meter"] = 1;
    cm_c["km"]    = 1/1000;
    cm_c["inch"]  = 100/2.54;
    cm_c["feet"]  = 100/(2.54*12);
    cm_c["yard"]  = 100/(2.54*12*3);
    
    conversion rad_c;    rad_c["radian"] = M_PI / 180;
    conversion deg_c;    deg_c["degree"] = 180 / M_PI;
    
    converter["inch"]  = inch_c;
    converter["feet"]  = feet_c;
    converter["cm"]    = cm_c;
    converter["m"]     = m_c;
    converter["degree"]= rad_c;
    converter["radian"]= deg_c;
}

void Parameter::convert_unit ( )     // if user specified inches and program calls for feet.
{
    // Go from Specified_unit to NewUnit :
    if (m_unit_specified.compare(m_requested_unit)==0)
        return;     /* scale is *1.  No action required. */
    m_f_value *= converter[m_unit_specified][m_requested_unit];
    m_i_value = round( m_f_value );
    m_unit_specified = m_requested_unit;
}

/****************************************************************************/
/*************************  Bidirectional Parameter  ************************/
/****************************************************************************/
BidirectionalParameter::BidirectionalParameter()
{
    
}
BidirectionalParameter::~BidirectionalParameter()
{
    
}
bool BidirectionalParameter::is_positive_direction()
{
    bool retval = false;
    std::smatch m;
    std::regex exp( m_positive_regexpression );
    bool match = std::regex_search( m_matches[0].str(),  m, exp );
    if (match)
        retval = true;
    return retval;
}

bool BidirectionalParameter::is_negative_direction()
{
    bool retval = false;
    std::smatch m;
    std::regex exp( m_negative_regexpression );
    bool match = std::regex_search( m_matches[0].str(),  m, exp );
    if (match)
        retval = true;
    return retval;
}

void BidirectionalParameter::construct_regex( )
{
    /* "move it 5 feet higher"      "move"      "5 feet higher"
       "raise it 2 feet"            "raise"     "2 feet"
     */
    m_regexpression  = get_numeral_regexpression( m_type );
    m_regexpression += m_permitted_units;
    m_regexpression += " ?(";
    m_regexpression += "("+m_positive_regexpression + ")|";
    m_regexpression += "("+m_negative_regexpression + ")";
    m_regexpression += ")";
}

/****************************************************************************/
/******************************  Length x Width  ****************************/
/****************************************************************************/

LengthByWidth::LengthByWidth()
{
    
}
LengthByWidth::~LengthByWidth()
{
    
}
void LengthByWidth::construct_regex( )
{
    //Parameter::construct_regex();
    string numexp = get_numeral_regexpression( m_type );
    //m_regexpression += m_permitted_units;
    m_regexpression += "(feet |foot )?";
    m_regexpression += "(";
    m_regexpression += m_name;
    m_regexpression += ")";
}
void  LengthByWidth::set_name()
{
    
}

/****************************************************************************/
/***************************  Length Width Height  **************************/
/****************************************************************************/
LengthWidthHeight::LengthWidthHeight()
{
    
}
LengthWidthHeight::~LengthWidthHeight()
{
    
}
void LengthWidthHeight::construct_regex()
{
    
}
void LengthWidthHeight::set_name()
{
    
}

