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

#ifndef _COMMAND_LINE_OPTIONS_HPP_
#define _COMMAND_LINE_OPTIONS_HPP_

// My attempt at a command line option parser/program initialiser thingo.
// 
// Mark Cox (2010)

/*
  Todo
 */


#include <stdexcept>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

// prototype template function for converting string arguments to non string types.
template <typename T> T convertOptionArgument(const std::string &argument);

class OptionDescription;
class OptionDescriptions;

class Option
{
public:
  Option(const OptionDescription &description);
   
  std::string argument() const;
  std::string argument(const std::string &defaultValue) const;
  std::string argument(const std::string &defaultValue, bool &is_present) const;

  std::string argument(const char *defaultValue) const;
  std::string argument(const char *defaultValue, bool &is_present) const;
  
  void arguments(std::vector<std::string> &storage) const;

  // same as argument(...) but passes the std::string through convertOptionArgument<T>(arg) before returning.
  template <typename T>
  T argument() const;

  template <typename T>
  T argument(T defaultValue) const;
  
  template <typename T>
  T argument(T defaultValue, bool &is_present) const;  


  template <typename T>
  void
  arguments(std::vector<T> &storage) const;
  
  const std::string &identifier() const;

  const OptionDescription &description;

  std::vector<std::string> option_arguments;
};

class Options
{
public:  
  Options();

  bool isPresent(const std::string &identifier) const;

  std::string argument(const std::string &identifier) const;  
  std::string argument(const std::string &identifier, const std::string &defaultValue) const;
  std::string argument(const std::string &identifier, const std::string &defaultValue, bool &is_present) const;

  std::string argument(const std::string &identifier, const char *defaultValue) const;
  std::string argument(const std::string &identifier, const char *defaultValue, bool &is_present) const;

  void arguments(const std::string &identifier, std::vector<std::string> &storage) const;

  template <typename T>
  T argument(const std::string &identifier) const;

  template <typename T>
  T argument(const std::string &identifier, T defaultValue) const;

  template <typename T>
  T argument(const std::string &identifier, T defaultValue, bool &is_present) const;

  template <typename T>
  void 
  arguments(const std::string &identifier, std::vector<T> &storage) const;
 
  const std::vector<std::string> &others() const;

  template <typename T>
  void
  others(std::vector<T> &others) const;

  const std::string &programInvocationString() const;  

  std::string program_invocation_string;

  typedef std::list<Option> OptionSequence;
  OptionSequence option_sequence;  

  std::vector<std::string> unassigned_options;

  OptionSequence::iterator findIdentifier(const std::string &identifier);
  OptionSequence::const_iterator findIdentifier(const std::string &identifier) const;

  OptionDescriptions *descriptions;
};

class OptionDescription
{
public:
  enum ArgumentDetail {
    ARGUMENT_NONE      = 0,
    ARGUMENT_IS_SWITCH = 0,
    ARGUMENT_OPTIONAL  = 1,
    ARGUMENT_REQUIRED  = 2,
    ARGUMENT_MULTIPLE  = 3
  };

  typedef std::list<std::string> IdentifierOptions;   // List of options for a given identifier

  OptionDescription(const std::string &identifier, ArgumentDetail detail);

  const std::string &identifier() const;
  const IdentifierOptions &options() const;

  void registerOption(const std::string &option);
  void clearOptions();

  void setArgumentDetail(ArgumentDetail detail);
  
  bool isRepetitionAllowed() const;
  bool isArgumentRequired() const;
  bool isArgumentOptional() const;
  bool isOptionASwitch() const;

  bool isConsistent() const;
  void assertIsConsistent() const;

  bool matchOption(std::string option) const;
 
  IdentifierOptions identifier_options;
  std::string identifier_label;
  ArgumentDetail argument_detail;
};

class OptionDescriptions
{
public:  
  typedef std::list<OptionDescription> DescriptionSequence;
  DescriptionSequence descriptions;

  OptionDescriptions();

  void registerIdentifier(const std::string &identifier, OptionDescription::ArgumentDetail detail = OptionDescription::ARGUMENT_NONE);
  void registerIdentifier(const std::string &identifier, const std::string &option, OptionDescription::ArgumentDetail detail = OptionDescription::ARGUMENT_NONE);
  void registerOption(const std::string &identifier, const std::string &option);    

  void processOptions(int argc, char **argv, Options &options);

  DescriptionSequence::iterator findDescription(const std::string &identifier);
  DescriptionSequence::const_iterator findDescription(const std::string &identifier) const;

  DescriptionSequence::const_iterator matchOption(const std::string &option) const;
  bool isIdentifierRegistered(const std::string &identifier) const;
  void assertIdentifierRegistered(const std::string &identifier) const;
};

class OptionNotPresent : public std::exception
{
public:
  OptionNotPresent(const std::string &identifier);
  ~OptionNotPresent() throw();
  
  const std::string &identifier() const;

  std::string option_identifier;
};

class OptionArgumentNotPresent : public std::exception
{
public:
  OptionArgumentNotPresent(const OptionDescription &description, const std::string &option_string);

  ~OptionArgumentNotPresent() throw();

  const std::string &identifier() const;
  const std::string &option() const;

  OptionDescription description;
  std::string option_string;
};

class InvalidSyntaxForOptionArgument : public std::exception
{
public:
  InvalidSyntaxForOptionArgument(const std::string &culprit, const std::string &attempting_verb);
  ~InvalidSyntaxForOptionArgument() throw();
  
  const std::string &culprit() const;
  const std::string &attemptingVerb() const;
  
  std::string bad_syntax_culprit;
  std::string attempting_verb;
};

class InvalidUsage : public std::exception
{
public:
  InvalidUsage();
  ~InvalidUsage() throw();
};

// helper functions for converting string arguments to other types.
template <typename T>
struct ConvertOptionArgumentFunctor
{
  T operator()(const std::string &argument) {
    return convertOptionArgument<T>(argument);
  }
};

template <> bool          convertOptionArgument(const std::string &argument);
template <> int           convertOptionArgument(const std::string &argument);
template <> float         convertOptionArgument(const std::string &argument);
template <> double        convertOptionArgument(const std::string &argument);
template <> unsigned int  convertOptionArgument(const std::string &argument);
template <> unsigned long convertOptionArgument(const std::string &argument);

#include "command-line-options.tpp"
#endif
