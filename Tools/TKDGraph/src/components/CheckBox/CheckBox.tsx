/** Dependencies */
import React from 'react';
import './CheckBox.css';
import checkedIcon from '../../../assets/icons/checked.png';

/**
 * @brief Checkbox component props
 * @description Interface defining the properties for the Checkbox component.
 */
export interface CheckBoxProps {
  checked: boolean;
  disabled?: boolean;
  onChange?: (checked: boolean) => void;
}

/**
 * @brief Checkbox component
 * @description A simple checkbox component that reflects its checked state and notifies on changes.
 */
export const CheckBox: React.FC<CheckBoxProps> = ({ checked, onChange, disabled = false }) => {
  /** Handle for the click event */
  const handleClick = () => {
    if (onChange && !disabled) {
      onChange(!checked);
    }
  };

  return (
    <div className={`ue-checkbox${disabled ? ' disabled' : ''}`} onClick={handleClick} aria-disabled={disabled}>
      {checked && <img src={checkedIcon} alt='Checked' draggable={false} />}
    </div>
  );
};
