#pragma once

#include <time.h>
#include <string>

/*! \enum RuleKind
	\brief	This enum represents the kind of every single recurrency rule of the event.
	\sa struct tm	
*/
typedef enum RULEKIND {

	kConstPeriod,	/*!< Event occurs every preset period of time.
						 In this case, only \c tm_hour, \c tm_min and \c tm_mday matter. 
						 The \c tm_day defines the period in days between starts of the Event. */

	kDaily,			/*!< The event occurs every day at a preset time.
						 Only \c tm_hour and \c tm_min matter. */

	kWeekly,		/*!< The pattern of occurrences repeats every week.
						 Example: "Every sunday, monday and friday".
						 This is the only kind of recurrence rule that requires a list of
						 struct tm datastructures. All others use only one struct tm.
						 Every single day requires a dedicated "struct tm" in the list.
						 Otherwise, the rule can be programmatically separated to up to 
						 7 rules, one for each day of week. Both ways are OK, but the first one
						 is preferred.
						 Only \c tm_hour, \c tm_min and \c tm_wday of every element in the list matter. */

	kMonthlyDates,	/*!< The pattern repeats itself on a specific date of the month.
						 Example: "Every 25th of the month".
						 Only \c tm_hour, \c tm_min and \c tm_mday of every element in the list matter. */

	kMonthlyRelative,	/*!< The pattern repeats itself on the relative date of every month.
						 Example: "Every last Monday", "Second from end Saturday".
						 Only \c tm_hour, \c tm_min, \c tm_wday and \c tm_mday of every element in
						 the list matter.
						 \c tm_mday is the week number, it can be one of the following:
						 {-5 (last from end = first), -4, -3, -2 , -1 (last), 1 (first), ... , 5 (last)}. */

	kYearlyDates,	/*!< The pattern repeats itself every year.
						 Just like MONTHLY_DATES, but this time tm_month is also used. */

	kYearlyRelative	/*!< The pattern repeats itself on the relative dates of every year.
						 The relative month is set by \c tm_month and can be everything from \c MIN_INT to
						 \c MAX_INT, excluding 0. Actual limits are set by the selected CalendarModule. 
						 If \c tm_month is 0, then this rule is ignored. */

} RuleKind;


/*! \enum		RecurrencyDefinition
	\brief		The enum defines how long the rule will be in effect.
 */
typedef enum RECURRENCY_DEFINITION {
	kInfinitely,		/*!< There is no limit to number of occurrences. */
	kOneTimeOnly,		/*!< The rule is one-timer. E. g., a rule defining a single omitting of a recurring Event. */
	kFixedNumber,		/*!< Ending after fixed number of occurrences.
							 The counter will be decreased only if an Event activity was started.
							 If an occurrence is omitted - it doesn't count. */
	kBySpecificDate		/*!< The event will end by a specific date - \a inclusive.
							 It means that if something recurs daily until Dec. 10th, there \a will \a be 
							 an occurrence on Dec. 10th. */
} RecurrencyDefinition;

/*! \union	EffectiveUntil
	\brief	This union holds either date of the last occurrence, or number of occurrences. */
typedef	union EFFECTIVE_UNTIL {
	struct tm		lastDate;		//!< Date of the last occurrence - in local calendar
	unsigned int	numOccurrences;	//!< Or, alternatively, amount of occurrences, which is not 0 and not 1.
} EffectiveUntil;

/*! \struct		Rule
	\brief		This structure defines a single rule.
	\details	Both rules of type "AND" and rules of type "NOT" are defined using this struct.
 */
typedef struct Rule {
	RuleKind kind;		/*!< The kind of the rule.
							 \sa enum RULEKIND */
	struct tm *dates;	//!< The list of \b beginnings of recurrences.
	int numOfElements;	//!< Number of elements in list \c dates.
	bool omitted;		//!< If this variable is "true", this rule is omitted.
	RecurrencyDefinition inEffect;	//!< Definition of rule's recurrency.
	EffectiveUntil until;	//!< Until when is the rule in effect?
} Rule;
