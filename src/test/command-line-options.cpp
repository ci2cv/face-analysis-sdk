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

#include "command-line-options.hpp"
#include <iostream>
#include <sstream>

//Utilities
template <typename Operation2, typename Operation1>
struct UnaryCompose : public std::unary_function<typename Operation1::argument_type, typename Operation2::result_type>
{
  UnaryCompose(Operation2 &op2, Operation1 &op1)
    : op1(op1), op2(op2)
  {

  }

  typename Operation2::result_type 
  operator()(typename Operation1::argument_type arg1) {
    typename Operation1::result_type r1 = op1(arg1);
    typename Operation2::result_type r2 = op2(r1);
    return r2;
  }

  Operation1 op1;
  Operation2 op2;
};

template <typename Operation2, typename Operation1>
UnaryCompose<Operation2, Operation1>
unary_compose(Operation2 op2, Operation1 op1)
{
  return UnaryCompose<Operation2,Operation1>(op2,op1);
}

//Option class
const std::string &
Option::identifier() const
{
  return description.identifier();
}

Option::Option(const OptionDescription &description)
  : description(description)
{

}

std::string
Option::argument() const
{
  if (option_arguments.empty() && description.isOptionASwitch())
    throw std::logic_error("Attempting to retrieve an argument from an option that is a switch.");
  else if (option_arguments.empty() && description.isArgumentOptional())
    throw OptionArgumentNotPresent(description,description.identifier());
  else if (option_arguments.empty() && description.isArgumentRequired())
    throw std::logic_error("Something is terribly wrong with the argument processing.");
  else if (option_arguments.size() > 1) {
    std::stringstream s;
    s << "More than one argument is available for option identifier: " << identifier();
    throw std::logic_error(s.str());
  } else if (option_arguments.size() == 1)
    return option_arguments[0];
  else
    throw std::logic_error("Should not get here.");    
}

std::string
Option::argument(const std::string &defaultValue) const
{
  bool is_present;
  return argument(defaultValue, is_present);
}

std::string
Option::argument(const std::string &defaultValue, bool &is_present) const
{
  if (option_arguments.empty() && description.isOptionASwitch())
    throw std::logic_error("Attempting to retrieve an argument from an option that is a switch.");
  else if (option_arguments.empty() && description.isArgumentOptional()) {
    is_present = false;
    return defaultValue;
  } else if (option_arguments.empty() && description.isArgumentRequired())
    throw std::logic_error("Something is terribly wrong with the argument processing.");
  else if (option_arguments.size() > 1) {
    std::stringstream s;
    s << "More than one argument is available for option identifier: " << identifier();
    throw std::logic_error(s.str());    
  } else if (option_arguments.size() == 1) {
    is_present = true;
    return option_arguments[0];
  } else
    throw std::logic_error("Should not get here.");    
}

std::string 
Option::argument(const char *defaultValue) const
{
  std::string v(defaultValue);
  return argument(v);
}

std::string 
Option::argument(const char *defaultValue, bool &is_present) const
{
  std::string v(defaultValue);
  return argument(v, is_present);
}

void
Option::arguments(std::vector<std::string> &storage) const
{
  storage = option_arguments;
}

//Options class
Options::Options()
  : descriptions(0)
{
  
}

std::string 
Options::argument(const std::string &identifier) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  if (it == option_sequence.end())
    throw OptionNotPresent(identifier);
  else
    return it->argument();
}

std::string 
Options::argument(const std::string &identifier, const std::string &defaultValue) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);  
  if (it == option_sequence.end())
    return defaultValue;
  else
    return it->argument(defaultValue);
}

std::string 
Options::argument(const std::string &identifier, const char *defaultValue) const
{
  std::string v(defaultValue);
  return argument(identifier, v);
}

std::string 
Options::argument(const std::string &identifier, const char *defaultValue, bool &is_present) const
{
  std::string v(defaultValue);
  return argument(identifier, v, is_present);
}

std::string 
Options::argument(const std::string &identifier, const std::string &defaultValue, bool &is_present) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);  
  if (it == option_sequence.end())
    return defaultValue;
  else
    return it->argument(defaultValue, is_present);
}

void 
Options::arguments(const std::string &identifier, std::vector<std::string> &storage) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  if (it == option_sequence.end())
    storage.clear();
  else
    it->arguments(storage);
}

bool 
Options::isPresent(const std::string &identifier) const
{
  OptionSequence::const_iterator it = findIdentifier(identifier);
  return it != option_sequence.end();
}

const std::vector<std::string> &
Options::others() const
{
  return unassigned_options;
}

const std::string &
Options::programInvocationString() const
{
  return program_invocation_string;
}

Options::OptionSequence::iterator
Options::findIdentifier(const std::string &identifier)
{
  descriptions->assertIdentifierRegistered(identifier);
  return std::find_if(option_sequence.begin(),
		      option_sequence.end(),
		      unary_compose(std::bind2nd(std::equal_to<std::string>(), identifier),
				    std::mem_fun_ref(&Option::identifier)));
}

Options::OptionSequence::const_iterator
Options::findIdentifier(const std::string &identifier) const
{
  descriptions->assertIdentifierRegistered(identifier);
  return std::find_if(option_sequence.begin(),
		      option_sequence.end(),
		      unary_compose(std::bind2nd(std::equal_to<std::string>(), identifier),
				    std::mem_fun_ref(&Option::identifier)));
}

//OptionDescription
OptionDescription::OptionDescription(const std::string &identifier, OptionDescription::ArgumentDetail detail)
  : identifier_label(identifier), argument_detail(detail)
{
  assertIsConsistent();
}

bool
OptionDescription::isConsistent() const
{
  return (((argument_detail == ARGUMENT_NONE)
	   || (argument_detail == ARGUMENT_OPTIONAL)
	   || (argument_detail == ARGUMENT_REQUIRED)
	   || (argument_detail == ARGUMENT_MULTIPLE)));
}

void
OptionDescription::assertIsConsistent() const
{
  if (!isConsistent())
    throw std::logic_error("OptionDescription instance has become inconsistent.");
}

const std::string &
OptionDescription::identifier() const
{
  return identifier_label;
}

bool
OptionDescription::matchOption(std::string option) const
{
  IdentifierOptions::const_iterator it = std::find_if(identifier_options.begin(),
						      identifier_options.end(),
						      std::bind2nd(std::equal_to<std::string>(), option));
  return it != identifier_options.end();
}

void
OptionDescription::registerOption(const std::string &option) 
{
  if (!matchOption(option))
    identifier_options.push_back(option);
}

bool
OptionDescription::isOptionASwitch() const
{
  assertIsConsistent();
  return (argument_detail == ARGUMENT_NONE);
}

bool
OptionDescription::isRepetitionAllowed() const
{
  assertIsConsistent();
  return (argument_detail == ARGUMENT_MULTIPLE);
}

bool
OptionDescription::isArgumentRequired() const
{
  assertIsConsistent();
  return ((argument_detail == ARGUMENT_REQUIRED)
	  || (argument_detail == ARGUMENT_MULTIPLE));
}

bool
OptionDescription::isArgumentOptional() const
{
  assertIsConsistent();
  return (argument_detail == ARGUMENT_OPTIONAL);
}


// OptionDescriptions
OptionDescriptions::OptionDescriptions()
{

}

void 
OptionDescriptions::registerIdentifier(const std::string &identifier, OptionDescription::ArgumentDetail detail)
{
  OptionDescription description(identifier, detail);
  DescriptionSequence::iterator it = findDescription(identifier);
  if (it != descriptions.end())
    throw std::logic_error("Registering an identifier twice.");

  descriptions.push_back(description);
}

void 
OptionDescriptions::registerIdentifier(const std::string &identifier, const std::string &option, OptionDescription::ArgumentDetail detail)
{
  registerIdentifier(identifier, detail);
  registerOption(identifier, option);
}

void
OptionDescriptions::registerOption(const std::string &identifier, const std::string &option)
{
  DescriptionSequence::iterator it = findDescription(identifier);
  if (it == descriptions.end())
    throw std::logic_error("Attempting to register an option for an unregistered identifier.");

  it->registerOption(option);
}

void
OptionDescriptions::processOptions(int argc, char **argv, Options &options)
{
  if (argc < 1) 
    throw std::logic_error("Invalid number arguments passed to processOpitons(...)");

  options.program_invocation_string = std::string(argv[0]);  
  options.unassigned_options.clear();  
  options.unassigned_options.reserve(argc);
  options.descriptions = this;

  for (int i = 1; i < argc; i++) {
    DescriptionSequence::const_iterator description_it = matchOption(argv[i]);
    if (description_it == descriptions.end()) {
      options.unassigned_options.push_back(argv[i]);
    } else {      
      // see if we have seen it before.
      Options::OptionSequence::iterator option_it = options.findIdentifier(description_it->identifier());
      // nope, haven't seen it.
      if (option_it == options.option_sequence.end()) {
	Option op(*description_it);
	if (description_it->isOptionASwitch()) {

	} else if (description_it->isArgumentOptional()) {
	  if (((i + 1) < argc) && (matchOption(argv[i+1]) == descriptions.end())) {
	    op.option_arguments.push_back(argv[i+1]);
	    i += 1;
	  } 
	} else if (description_it->isArgumentRequired()) {
	  if (((i + 1) >= argc) || (matchOption(argv[i+1]) != descriptions.end())) {
	    throw OptionArgumentNotPresent(*description_it, argv[i]);
	  } else {
	    op.option_arguments.push_back(argv[i+1]);
	    i += 1;
	  }
	} else {
	  throw std::logic_error("OptionDescription is somehow in consistent.");
	}
	options.option_sequence.push_back(op);
      } else { // yup, we have seen it. 
	if (description_it->isRepetitionAllowed()) {
	  if (((i + 1) >= argc) || (matchOption(argv[i+1]) != descriptions.end())) {
	    throw OptionArgumentNotPresent(*description_it, argv[i]);
	  } else {
	    option_it->option_arguments.push_back(argv[i+1]);
	    i += 1;
	  }
	} else {
	  throw InvalidUsage();
	}
      }
    }
  }
}

OptionDescriptions::DescriptionSequence::iterator
OptionDescriptions::findDescription(const std::string &identifier) 
{
  return std::find_if(descriptions.begin(),
		      descriptions.end(),
		      unary_compose(std::bind2nd(std::equal_to<std::string>(), identifier),
				    std::mem_fun_ref(&OptionDescription::identifier)));
}

OptionDescriptions::DescriptionSequence::const_iterator
OptionDescriptions::findDescription(const std::string &identifier) const
{
  return std::find_if(descriptions.begin(),
		      descriptions.end(),
		      unary_compose(std::bind2nd(std::equal_to<std::string>(), identifier),
				    std::mem_fun_ref(&OptionDescription::identifier)));
}

OptionDescriptions::DescriptionSequence::const_iterator
OptionDescriptions::matchOption(const std::string &option) const
{  
  return std::find_if(descriptions.begin(),
		      descriptions.end(),
		      std::bind2nd(std::mem_fun_ref(&OptionDescription::matchOption),
				   option));
}

bool 
OptionDescriptions::isIdentifierRegistered(const std::string &identifier) const
{
  DescriptionSequence::const_iterator it = findDescription(identifier);
  return it != descriptions.end();
}

void 
OptionDescriptions::assertIdentifierRegistered(const std::string &identifier) const
{
  if (!isIdentifierRegistered(identifier)) {
    std::stringstream s;
    s << "Option identifier " << identifier << " is not registered.";
    throw std::logic_error(s.str());
  }
}

// OptionArgumentNotPresent
OptionArgumentNotPresent::OptionArgumentNotPresent(const OptionDescription &description, const std::string &option_string)
  : description(description), option_string(option_string)
{

}

OptionArgumentNotPresent::~OptionArgumentNotPresent() throw()
{

}

const std::string & 
OptionArgumentNotPresent::identifier() const
{
  return description.identifier();
}

const std::string &
OptionArgumentNotPresent::option() const
{
  return option_string;
}

//OptionNotPresent
OptionNotPresent::OptionNotPresent(const std::string &identifier)
  : option_identifier(identifier)
{

}

OptionNotPresent::~OptionNotPresent() throw()
{

}
  
const std::string &
OptionNotPresent::identifier() const
{
  return option_identifier;
}

//Invalid Usage
InvalidUsage::InvalidUsage()
{

}

InvalidUsage::~InvalidUsage() throw()
{

}

//InvalidSyntaxForOptionArgument
InvalidSyntaxForOptionArgument::InvalidSyntaxForOptionArgument(const std::string &culprit, const std::string &attempting_verb)
  : bad_syntax_culprit(culprit), attempting_verb(attempting_verb)
{

}

InvalidSyntaxForOptionArgument::~InvalidSyntaxForOptionArgument() throw()
{
  
}
  
const std::string &
InvalidSyntaxForOptionArgument::culprit() const
{
  return bad_syntax_culprit;
}

const std::string &
InvalidSyntaxForOptionArgument::attemptingVerb() const
{
  return attempting_verb;
}

// helper functions for converting strings to other types.
template <>
int
convertOptionArgument<int>(const std::string &argument)
{
  std::stringstream s(argument);
  s.exceptions(std::stringstream::badbit);

  int r;
  std::string word;
  try {
    s >> r;
    if (s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Integer option argument does not begin with an integer.");
    s >> word;
    if (!s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Contains rubbish not needed for an integer.");
  } catch (std::stringstream::failure &e) {
    std::stringstream msg;
    msg << "Can not convert option argument string \"" << argument << "\" to an integer. Reason: " << e.what();
    throw InvalidSyntaxForOptionArgument(argument, msg.str());
  }
  return r;
}

template <>
unsigned int
convertOptionArgument<unsigned int>(const std::string &argument)
{
  std::stringstream s(argument);
  s.exceptions(std::stringstream::badbit);

  unsigned int r;
  std::string word;
  try {
    s >> r;
    if (s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Unsigned option argument begins with a string, or a minus sign.");
    s >> word;
    if (!s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Contains rubbish not needed for an integer.");
  } catch (std::stringstream::failure &e) {
    std::stringstream msg;
    msg << "Can not convert option argument string \"" << argument << "\" to an unsigned integer. Reason: " << e.what();
    throw InvalidSyntaxForOptionArgument(argument, msg.str());
  }
  return r;
}

template <>
float
convertOptionArgument<float>(const std::string &argument)
{
  std::stringstream s(argument);
  s.exceptions(std::stringstream::badbit);

  float r;
  std::string word;
  try {
    s >> r;
    if (s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Float option argument begins with a string.");
    s >> word;
    if (!s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Contains rubbish not needed for a float.");
  } catch (std::stringstream::failure &e) {
    std::stringstream msg;
    msg << "Can not convert option argument string \"" << argument << "\" to a float. Reason: " << e.what();
    throw InvalidSyntaxForOptionArgument(argument, msg.str());
  }
  return r;
}

template <>
double
convertOptionArgument<double>(const std::string &argument)
{
  std::stringstream s(argument);
  s.exceptions(std::stringstream::badbit);

  double r;
  std::string word;
  try {
    s >> r;
    if (s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Double argument begins with a string.");
    s >> word;
    if (!s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Contains rubbish not needed for a double.");
  } catch (std::stringstream::failure &e) {
    std::stringstream msg;
    msg << "Can not convert option argument string \"" << argument << "\" to a double. Reason: " << e.what();
    throw InvalidSyntaxForOptionArgument(argument, msg.str());
  }
  return r;
}

template <>
bool
convertOptionArgument<bool>(const std::string &argument)
{
  std::string upcase;
  upcase.resize(argument.size());
  
  for (size_t i = 0; i < argument.size(); i++)
    upcase[i] = std::toupper(argument[i]);

  if ((upcase == "YES") || (upcase == "TRUE"))
    return true;
  else if ((upcase == "NO") || (upcase == "FALSE"))
    return false;

  unsigned int r = convertOptionArgument<unsigned int>(argument);
  if (r == 0)
    return false;
  else if (r == 1)
    return true;
  else {
    std::stringstream s;
    s << "Unable to convert bool option argument \"" << argument << "\" to a boolean value.";
    throw InvalidSyntaxForOptionArgument(argument, s.str());
  }
}


template <>
unsigned long
convertOptionArgument<unsigned long>(const std::string &argument)
{
  std::stringstream s(argument);
  s.exceptions(std::stringstream::badbit);

  unsigned long r;
  std::string word;
  try {
    s >> r;
    if (s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Unsigned long option argument begins with a string.");
    s >> word;
    if (!s.fail())
      throw InvalidSyntaxForOptionArgument(argument, "Contains rubbish not needed for an unsigned long.");
  } catch (std::stringstream::failure &e) {
    std::stringstream msg;
    msg << "Can not convert option argument string \"" << argument << "\" to an unsigned long. Reason: " << e.what();
    throw InvalidSyntaxForOptionArgument(argument, msg.str());
  }
  return r;
}
