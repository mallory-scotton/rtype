///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
/// \brief Safely deletes a dynamically allocated object and sets the pointer
/// to `nullptr` to prevent dangling pointers.
///
/// This macro ensures that after an object is deleted, the pointer is
/// immediately set to `nullptr`, reducing the risk of accidental dereferencing
/// of freed memory.
///
/// \param x Pointer to the object to be deleted.
///
///////////////////////////////////////////////////////////////////////////////
#define SafeDelete(x) if (x) { delete x; x = nullptr; }

///////////////////////////////////////////////////////////////////////////////
/// \brief Safely deletes a dynamically allocated array and sets the pointer
/// to `nullptr` to prevent dangling pointers.
///
/// Similar to `SafeDelete`, this macro deletes a dynamically allocated array
/// (created using `new[]`) and sets the pointer to `nullptr` to ensure that
/// the pointer cannot be used after deletion, avoiding potential errors.
///
/// \param x Pointer to the array to be deleted.
///
///////////////////////////////////////////////////////////////////////////////
#define SafeDeleteArray(x) if (x) { delete[] x; x = nullptr; }
