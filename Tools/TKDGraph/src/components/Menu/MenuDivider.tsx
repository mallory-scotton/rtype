/** Dependencies */
import React from 'react';

/**
 * @brief Menu Divider component props
 * @description The props for the Menu Divider component.
 */
export interface MenuDividerProps {
  label: string;
}

/**
 * @brief Menu Divider component
 * @description A component that represents a menu Divider in the UI.
 */
export const MenuDivider: React.FC<MenuDividerProps> = ({ label }) => {
  return <div className='ue-menu-divider'><span>{label}</span></div>;
};
