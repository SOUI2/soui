
#ifndef __base_auto_reset_h__
#define __base_auto_reset_h__

#pragma once

// AutoReset<> is useful for setting a variable to a new value only within a
// particular scope. An AutoReset<> object resets a variable to its original
// value upon destruction, making it an alternative to writing "var = false;"
// or "var = old_val;" at all of a block's exit points.
//
// This should be obvious, but note that an AutoReset<> instance should have a
// shorter lifetime than its scoped_variable, to prevent invalid memory writes
// when the AutoReset<> object is destroyed.
template<typename T>
class AutoReset
{
public:
    AutoReset(T* scoped_variable, T new_value)
        : scoped_variable_(scoped_variable),
        original_value_(*scoped_variable)
    {
        *scoped_variable_ = new_value;
    }

    ~AutoReset() { *scoped_variable_ = original_value_; }

private:
    T* scoped_variable_;
    T original_value_;

    DISALLOW_COPY_AND_ASSIGN(AutoReset);
};

#endif //__base_auto_reset_h__