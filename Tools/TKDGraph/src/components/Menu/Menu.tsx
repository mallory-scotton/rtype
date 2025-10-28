/** Dependencies */
import React from 'react';
import './Menu.css';
import './MenuItem';

/**
 * @brief Menu component props
 * @description The props for the Menu component.
 */
export interface MenuProps {
  children?: React.ReactNode[] | React.ReactNode;
}

/**
 * @brief Menu component
 * @description A component that represents a menu in the UI.
 */
export const Menu: React.FC<MenuProps> = ({ children }) => {
  return <div className='ue-menu'>{children}</div>;
};
