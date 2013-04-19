// CSIRO has filed various patents which cover the Software. 

// CSIRO grants to you a license to any patents granted for inventions
// implemented by the Software for academic, research and non-commercial
// use only.

// CSIRO hereby reserves all rights to its inventions implemented by the
// Software and any patents subsequently granted for those inventions
// that are not expressly granted to you.  Should you wish to license the
// patents relating to the Software for commercial use please contact
// CSIRO IP & Licensing, Gautam Tendulkar (gautam.tendulkar@csiro.au) or
// Nick Marsh (nick.marsh@csiro.au)

// This software is provided under the CSIRO OPEN SOURCE LICENSE
// (GPL2) which can be found in the LICENSE file located in the top
// most directory of the source code.

// Copyright CSIRO 2013

// -*-c++-*-
#ifndef _COMMAND_LINE_OPTIONS_TPP_
#define _COMMAND_LINE_OPTIONS_TPP_

template <typename T>
T
Option::argument(T defaultValue, bool &is_present) const
{
  std::string arg_string = argument("", is_present);
  if (is_present) {
    return convertOptionArgument<T>(arg_string);
  } else {
    return defaultValue;
  }
}

template <typename T>
T
Options::argument(const std::string &identifier) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  if (it == option_sequence.end()) {
    throw OptionNotPresent(identifier);
  } else {    
    return convertOptionArgument<T>(it->argument());
  }
}

template <typename T>
T
Options::argument(const std::string &identifier, T defaultValue) const
{
  bool is_present;
  return argument<T>(identifier, defaultValue, is_present);
}

template <typename T>
T
Options::argument(const std::string &identifier, T defaultValue, bool &is_present) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  if (it == option_sequence.end()) {
    return defaultValue;
  } else {
    return it->argument<T>(defaultValue, is_present);
  }   
}

template <typename T>
void 
Options::arguments(const std::string &identifier, std::vector<T> &storage) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  if (it == option_sequence.end())
    storage.clear();
  else
    it->arguments(storage);
}

template <typename T>
void
Option::arguments(std::vector<T> &storage) const
{
  storage.clear();
  storage.resize(option_arguments.size());
  std::transform(option_arguments.begin(),
		 option_arguments.end(),
		 storage.begin(),
		 ConvertOptionArgumentFunctor<T>());
}

template <typename T>
void
Options::others(std::vector<T> &others) const
{
  std::vector<std::string> str_others;
  str_others = this->others();
  
  others.resize(str_others.size());
  std::transform(str_others.begin(), str_others.end(), 
		 others.begin(),
		 ConvertOptionArgumentFunctor<T>());
}

#endif
