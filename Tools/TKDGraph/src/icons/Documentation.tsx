/** Dependencies */
import React from 'react';
import type { IconProps } from './shared';

/**
 * @brief Icon Component
 * @description A React component that renders the "Documentation" icon.
 */
export const Documentation: React.FC<IconProps> = ({ width = 16, height = 16, color = '#FFFFFF' }) => {
  return (
    <svg width={width} height={height} viewBox='0 0 16 16' fill='none' xmlns='http://www.w3.org/2000/svg'>
      <path fillRule='evenodd' clipRule='evenodd' d='M15 1H6L0 10H10L15 1Z' fill={color} />
      <path d='M14.5 6L10 14.1H1V10.5H0V15H11L16 6H14.5Z' fill={color} />
      <path opacity='0.5' d='M9 11H2V13H9V11Z' fill={color} />
    </svg>
  );
};
