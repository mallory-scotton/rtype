/** Dependencies */
import React from 'react';
import './Tab.css';
import Cross from '../../../assets/icons/cross.png';

/**
 * @brief Tab component props
 * @description Props for the Tab component including title, active state, and click handler.
 */
export interface TabProps {
  title: string;
  size?: 'default' | 'large';
  color?: 'default' | 'blueprint' | 'material' | 'level' | 'mesh';
  icon?: React.ReactNode;
  selected?: boolean;
  onClick?: () => void;
  dirty?: boolean;
}

/**
 * @brief Tab component
 * @description A clickable tab component that displays a title and indicates if it is selected.
 */
export const Tab: React.FC<TabProps> = ({
  title,
  selected = false,
  size = 'default',
  icon,
  onClick,
  dirty = false,
  color = 'default'
}) => {
  // State to track hover status
  const [hovered, setHovered] = React.useState(false);

  // Render the tab with appropriate styles and event handlers
  return (
    <div
      className={`ue-tab${selected ? ' selected' : ''}${size === 'large' ? ' large' : ''}${dirty ? ' dirty' : ''}`}
      data-selected={selected}
      data-size={size}
      onClick={onClick}
      onMouseEnter={() => setHovered(true)}
      onMouseLeave={() => setHovered(false)}
    >
      <span className='ue-tab-title'>
        {icon && <span className={`ue-tab-icon ${color}`}>{icon}</span>}
        {title}
        {dirty && ' *'}
      </span>
      {(selected || hovered) && <img className='ue-tab-close' src={Cross} alt='Cross' />}
    </div>
  );
};
