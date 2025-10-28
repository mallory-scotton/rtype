/** Dependencies */
import React, { useEffect, useState } from 'react';
import { useEditor } from '../../context';
import { computeConnectionRenderData } from '../../utils';
import type { Connection as ConnectionType } from '../../types';
import './Connection.css';

/**
 * @brief Connection component props
 */
interface ConnectionProps {
  connection: ConnectionType;
}

/**
 * @brief Connection component
 * @description Renders an SVG connection line between two pins. Self-contained with context.
 */
export const Connection: React.FC<ConnectionProps> = ({ connection }) => {
  const {
    canvasRef,
    canvasTransform,
    deleteConnection,
    connectionUpdateTrigger,
    isAltPressed,
    hoveredConnectionId,
    setHoveredConnectionId
  } = useEditor();

  const [renderData, setRenderData] = useState<{
    path: string;
    color: string;
    x: number;
    y: number;
    width: number;
    height: number;
  } | null>(null);

  // Compute connection render data
  useEffect(() => {
    if (!canvasRef.current) return;

    const computeRenderData = () => {
      if (!canvasRef.current) return;

      const sourcePin = canvasRef.current.querySelector(
        `.pin[data-id="${connection.sourcePinId}"] .clink`
      ) as HTMLElement;
      const targetPin = canvasRef.current.querySelector(
        `.pin[data-id="${connection.targetPinId}"] .clink`
      ) as HTMLElement;

      if (sourcePin && targetPin) {
        const data = computeConnectionRenderData(sourcePin, targetPin, canvasTransform.scale);
        if (data) {
          setRenderData(data);
        }
      }
    };

    // Use requestAnimationFrame to ensure DOM is updated
    requestAnimationFrame(computeRenderData);
  }, [connection, canvasTransform, canvasRef, connectionUpdateTrigger]);

  const handleClick = (e: React.MouseEvent) => {
    e.stopPropagation();

    // Only delete if ALT is pressed
    if (isAltPressed) {
      deleteConnection(connection.id);
    }
  };

  const handleMouseEnter = () => {
    if (isAltPressed) {
      setHoveredConnectionId(connection.id);
    }
  };

  const handleMouseLeave = () => {
    setHoveredConnectionId(null);
  };

  if (!renderData) {
    return null;
  }

  const isHovered = hoveredConnectionId === connection.id;
  const isDimmed = isAltPressed && hoveredConnectionId !== null && !isHovered;

  // Stroke width: 4 when hovered with ALT, otherwise 2
  const strokeWidth = isHovered && isAltPressed ? '4' : '2';
  const opacity = isDimmed ? 0.3 : 1;

  return (
    <svg
      className='link'
      data-id={connection.id}
      style={{
        transform: `translate(${renderData.x}px, ${renderData.y}px)`,
        position: 'absolute',
        pointerEvents: 'none'
      }}
      width={renderData.width}
      height={renderData.height}
    >
      <path
        className='connection-path'
        d={renderData.path}
        fill='none'
        stroke={`#${renderData.color}`}
        strokeWidth={strokeWidth}
        pointerEvents='visibleStroke'
        onClick={handleClick}
        onMouseEnter={handleMouseEnter}
        onMouseLeave={handleMouseLeave}
        style={{
          cursor: isAltPressed ? 'pointer' : 'default',
          opacity
        }}
      />
    </svg>
  );
};
