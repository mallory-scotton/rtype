/** Dependencies */
import React from 'react';

/**
 * @brief Connection component props
 */
interface ConnectionProps {
  path: string;
  color: string;
  x: number;
  y: number;
  width: number;
  height: number;
  connectionId: string;
  isSelected?: boolean;
  isDimmed?: boolean;
  onClick?: (connectionId: string) => void;
}

/**
 * @brief Connection component
 * @description Renders an SVG connection line between two pins
 */
export const Connection: React.FC<ConnectionProps> = ({
  path,
  color,
  x,
  y,
  width,
  height,
  connectionId,
  isSelected = false,
  isDimmed = false,
  onClick
}) => {
  const handleClick = (e: React.MouseEvent) => {
    e.stopPropagation();
    if (onClick) {
      onClick(connectionId);
    }
  };

  return (
    <svg
      className='link'
      data-id={connectionId}
      style={{
        transform: `translate(${x}px, ${y}px)`,
        position: 'absolute',
        pointerEvents: 'none',
        opacity: isDimmed ? 0.3 : 1
      }}
      width={width}
      height={height}
    >
      <path
        d={path}
        fill='none'
        stroke={`#${color}`}
        strokeWidth={isSelected ? '4' : '2'}
        pointerEvents='visibleStroke'
        onClick={handleClick}
        style={{ cursor: 'default' }}
      />
    </svg>
  );
};
