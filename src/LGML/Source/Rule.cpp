/*
  ==============================================================================

    Rule.cpp
    Created: 4 May 2016 5:06:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "Rule.h"

Rule::Rule(const String &_name) :
	ControllableContainer(_name),
	activationType(OnActivate)
{
	rootConditionGroup = new RuleConditionGroup(nullptr);
	rootConditionGroup->addCondition();

	nameParam = addStringParameter("Name", "Name of the rule", _name);
	enabledParam = addBoolParameter("Enabled", "Enable / Disable the rule",true);

	isActiveParam = addBoolParameter("active", "Whether the rule is active or not.\nA rule is active if its conditions are validated.", false);
	isActiveParam->isControllableFeedbackOnly = true;
}

Rule::~Rule()
{
}

ControlVariableReference * Rule::addReference()
{
	ControlVariableReference * cvr = new ControlVariableReference();
	references.add(cvr);
	cvr->addReferenceListener(this);
	ruleListeners.call(&RuleListener::referenceAdded, cvr);
	return cvr;
}

void Rule::removeReference(ControlVariableReference * cvr)
{
	cvr->removeReferenceListener(this);
	ruleListeners.call(&RuleListener::referenceRemoved, cvr);
	references.removeObject(cvr);
}

void Rule::addConsequence()
{
	RuleConsequence * c = new RuleConsequence();
	consequences.add(c);
	ruleListeners.call(&RuleListener::consequenceAdded,c);
}

void Rule::removeConsequence(RuleConsequence * c)
{
	ruleListeners.call(&RuleListener::consequenceRemoved, c);
	consequences.removeObject(c);

}

void Rule::onContainerParameterChanged(Parameter * p)
{
	if (p == nameParam)
	{
		setNiceName(nameParam->stringValue());
	}else if (p == isActiveParam)
	{
		ruleListeners.call(&RuleListener::ruleActivationChanged, this);
	}
}

void Rule::askForRemoveReference(ControlVariableReference * r)
{
	removeReference(r);
}


void Rule::remove()
{
	ruleListeners.call(&RuleListener::askForRemoveRule, this);
}
