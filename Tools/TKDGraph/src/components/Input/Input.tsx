/** Dependencies */
import React from 'react';
import './Input.css';

/**
 * @brief Input component props
 * @description Interface defining the properties for the Input component.
 */
export type InputProps =
  | {
      type: 'text';
      value?: string;
      placeholder?: string;
      disabled?: boolean;
      onChange?: (value: string) => void;
    }
  | {
      type: 'float' | 'number';
      value?: number;
      placeholder?: string;
      disabled?: boolean;
      onChange?: (value: number) => void;
    };

/**
 * @brief Input component
 * @description A simple input component.
 */
export const Input: React.FC<InputProps> = (props) => {
  const { type, value, placeholder, disabled, onChange } = props;

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (!onChange) return;

    if (type === 'number' || type === 'float') {
      const inputValue = e.target.value;
      // Allow empty string during typing
      if (inputValue === '') {
        (onChange as (value: number) => void)(0);
      } else {
        const parsedValue = type === 'float' ? parseFloat(inputValue) : parseInt(inputValue, 10);
        (onChange as (value: number) => void)(isNaN(parsedValue) ? 0 : parsedValue);
      }
    } else {
      (onChange as (value: string) => void)(e.target.value);
    }
  };

  return (
    <input
      className='ue-input'
      aria-disabled={disabled}
      data-type={type}
      data-value={value}
      type={type === 'number' || type === 'float' ? 'number' : 'text'}
      value={value ?? ''}
      placeholder={placeholder}
      onChange={handleChange}
      disabled={disabled}
    />
  );
};
