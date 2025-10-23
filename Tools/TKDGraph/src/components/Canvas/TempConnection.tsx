/** Dependencies */
import React, { useEffect, useState } from 'react';
import { useEditor } from '../../context';
import { computeTempConnectionRenderData } from '../../utils';
import './Connection.css';

/**
 * @brief TempConnection component
 * @description Renders a temporary SVG connection line while dragging from a pin.
 */
export const TempConnection: React.FC = () => {
  const { connectingFrom, mousePosition, canvasTransform } = useEditor();

  const [renderData, setRenderData] = useState<{
    path: string;
    color: string;
    x: number;
    y: number;
    width: number;
    height: number;
  } | null>(null);

  // Track mouse movement and recompute temp connection
  useEffect(() => {
    if (!connectingFrom || !mousePosition) {
      setRenderData(null);
      return;
    }

    const handleMouseMove = (event: MouseEvent) => {
      if (!connectingFrom) return;

      const data = computeTempConnectionRenderData(
        connectingFrom.element,
        { x: event.clientX, y: event.clientY },
        canvasTransform,
        connectingFrom.direction
      );

      if (data) {
        setRenderData(data);
      }
    };

    handleMouseMove({ clientX: mousePosition.x, clientY: mousePosition.y } as MouseEvent);

    window.addEventListener('mousemove', handleMouseMove);

    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
    };
  }, [connectingFrom, mousePosition, canvasTransform]);

  // Handle mouse up to clear connecting state
  useEffect(() => {
    if (!connectingFrom) return;

    const handleMouseUp = () => {
      setRenderData(null);
    };

    window.addEventListener('mouseup', handleMouseUp);

    return () => {
      window.removeEventListener('mouseup', handleMouseUp);
    };
  }, [connectingFrom]);

  if (!renderData || !connectingFrom) {
    return null;
  }

  return (
    <svg
      className='link temp'
      style={{
        transform: `translate(${renderData.x}px, ${renderData.y}px)`,
        position: 'absolute',
        pointerEvents: 'none',
        opacity: 0.5
      }}
      width={renderData.width}
      height={renderData.height}
    >
      <path d={renderData.path} fill='none' stroke={`#${renderData.color}`} strokeWidth='2' pointerEvents='none' />
    </svg>
  );
};
