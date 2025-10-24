/** Dependencies */
import React from 'react';

/**
 * @brief Menu Selection component props
 * @description The props for the Menu Selection component.
 */
export interface MenuSelectionProps {
  label: string;
  shortcut?: string;
  logo?: React.ReactNode;
}

/**
 * @brief Menu Selection component
 * @description A component that represents a menu Selection in the UI.
 */
export const MenuSelection: React.FC<MenuSelectionProps> = ({ logo, label, shortcut }) => {
  return (
    <div className='ue-menu-selection'>
      <span className='ue-menu-selection-label'>
        {logo && logo}
        {label}
      </span>
      <span className='ue-menu-selection-shortcut'>{shortcut}</span>
    </div>
  );
};
