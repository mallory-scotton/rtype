/** Dependencies */
import React from 'react';

/**
 * @brief Menu item component props
 * @description The props for the Menu item component.
 */
export interface MenuItemProps {
  label: string;
  selected?: boolean;
  children?: React.ReactNode[] | React.ReactNode;
  onSelect?: (name: string) => void;
  onHover?: (name: string) => void;
}

/**
 * @brief Menu item component
 * @description A single item within a menu.
 */
export const MenuItem: React.FC<MenuItemProps> = ({ label, selected, children, onSelect, onHover }) => {
  return (
    <div
      className={`ue-menu-item${selected ? ' selected' : ''}`}
      onClick={() => {
        onSelect?.(label);
      }}
      onMouseOver={() => {
        onHover?.(label);
      }}
    >
      {label}
      {selected && <div className='ue-menu-sub-element'>{children}</div>}
    </div>
  );
};
