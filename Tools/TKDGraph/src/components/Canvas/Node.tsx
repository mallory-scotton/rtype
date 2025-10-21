/** Dependencies */
import type { NodeData, PinDirection } from '../../types';
import { Pin } from './Pin';
import { useState, useRef, useEffect } from 'react';
import { snapToGrid } from '../../utils';
import './Node.css';

/**
 * @brief Interface for Node component props
 * @description This interface defines the structure of the props that the Node component expects to receive.
 */
interface NodeProps {
  data: NodeData;
  position: { x: number; y: number };
  dimension?: { width: number; height: number };
  selected?: boolean;
  onPositionChange?: (nodeId: string, position: { x: number; y: number }) => void;
  onDragStart?: (nodeId: string) => void;
  onDragEnd?: (nodeId: string) => void;
  onConnectionStart?: (pinId: string, direction: PinDirection, pinType: string) => void;
  onConnectionEnd?: (pinId: string, direction: PinDirection, pinType: string) => void;
  onPinHover?: (pinId: string, direction: PinDirection, pinType: string, isHovering: boolean) => void;
  onDisruptConnection?: (pinId: string, direction: PinDirection, pinType: string) => void;
  onPinValueChange?: (pinId: string, value: any) => void;
  onClick?: (nodeId: string, event?: React.MouseEvent) => void;
  scale?: number;
  isCtrlPressed?: boolean;
}

/**
 * @brief Node component
 * @description This component represents a node in a graph, displaying its data.
 */
export const Node: React.FC<NodeProps> = ({
  data,
  position,
  dimension,
  selected,
  onPositionChange,
  onDragStart,
  onDragEnd,
  onConnectionStart,
  onConnectionEnd,
  onPinHover,
  onDisruptConnection,
  onPinValueChange,
  onClick,
  scale = 1,
  isCtrlPressed = false
}) => {
  const classes: string[] = ['node'];
  const [isDragging, setIsDragging] = useState(false);
  const [currentPosition, setCurrentPosition] = useState(position);
  const dragStartPos = useRef({ x: 0, y: 0 });
  const nodeStartPos = useRef({ x: 0, y: 0 });

  // Update position when prop changes (from external source)
  useEffect(() => {
    setCurrentPosition(position);
  }, [position]);

  let allPins = [...(data.inputs || []), ...(data.outputs || [])];
  let nodeType = allPins.find((pin) => pin.type !== 'exec')?.type || '';

  if (selected) {
    classes.push('selected');
  }

  if (data.type === 'getter') {
    classes.push(`nvariableget connector ${nodeType}`);
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'setter') {
    classes.push(`nvariableset connector ${nodeType}`);
    data.roundedBg = data.roundedBg ?? 'SET';
    data.roundedBgColor = data.roundedBgColor ?? true;
  } else if (data.type === 'cast') {
    classes.push('nconv');
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'operator') {
    classes.push('nkismetmath');
    data.roundedBg = data.roundedBg ?? true;
  } else if (data.type === 'comment') {
    classes.push('ncomment');
  }

  const handleMouseDown = (event: React.MouseEvent) => {
    // Only handle left mouse button and not on pins
    if (event.button !== 0 || (event.target as HTMLElement).closest('.pin')) {
      return;
    }

    event.stopPropagation();
    onClick?.(data.id, event);
    setIsDragging(true);
    dragStartPos.current = { x: event.clientX, y: event.clientY };
    nodeStartPos.current = { ...currentPosition };
    onDragStart?.(data.id);
  };

  useEffect(() => {
    if (!isDragging) return;

    const handleMouseMove = (event: MouseEvent) => {
      const deltaX = (event.clientX - dragStartPos.current.x) / scale;
      const deltaY = (event.clientY - dragStartPos.current.y) / scale;

      const newX = nodeStartPos.current.x + deltaX;
      const newY = nodeStartPos.current.y + deltaY;

      setCurrentPosition({ x: newX, y: newY });
      // Update position immediately during drag (don't wait for mouse up)
      onPositionChange?.(data.id, { x: newX, y: newY });
    };

    const handleMouseUp = (event: MouseEvent) => {
      setIsDragging(false);

      // Calculate final position based on mouse position at mouseup (not currentPosition)
      const deltaX = (event.clientX - dragStartPos.current.x) / scale;
      const deltaY = (event.clientY - dragStartPos.current.y) / scale;
      const finalX = nodeStartPos.current.x + deltaX;
      const finalY = nodeStartPos.current.y + deltaY;

      // Snap to grid on mouse up
      const snappedX = snapToGrid(finalX, 16);
      const snappedY = snapToGrid(finalY, 16);

      setCurrentPosition({ x: snappedX, y: snappedY });
      onPositionChange?.(data.id, { x: snappedX, y: snappedY });
      onDragEnd?.(data.id);
    };

    window.addEventListener('mousemove', handleMouseMove);
    window.addEventListener('mouseup', handleMouseUp);

    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleMouseUp);
    };
  }, [isDragging, data.id, onPositionChange, onDragEnd, scale]);

  return (
    <div
      data-id={data.id}
      style={{
        position: 'absolute',
        transform: `translate(${currentPosition.x}px, ${currentPosition.y}px)`,
        width: dimension?.width,
        height: dimension?.height,
        backgroundColor: data.backgroundColor,
        cursor: isDragging ? 'grabbing' : 'grab'
      }}
      className={classes.join(' ')}
      onMouseDown={handleMouseDown}
    >
      {data.header && data.type !== 'comment' && (
        <div className={`header node-color ${data.header.type} gradient`}>
          {data.header.icon && <div className={`icon ${data.header.icon}`}></div>}
          {data.header.label && (
            <span className={`${data.header.icon ? 'has-icon ' : ''}name`}>{data.header.label}</span>
          )}
        </div>
      )}
      {data.type === 'comment' && data.header && (
        <div className='header' style={{ backgroundColor: data.header.backgroundColor }}>
          {data.header.label}
        </div>
      )}
      <div className='body'>
        {data.label && data.type === 'operator' && (
          <div className='center-text'>
            <span>{data.label}</span>
          </div>
        )}
        {data.roundedBgColor && <div className={`round-bg-color connector ${nodeType}`}></div>}
        {data.roundedBg && <div className='round-bg'>{typeof data.roundedBg === 'string' ? data.roundedBg : ''}</div>}
        <div className='left-col'>
          {data.inputs &&
            data.inputs.map((input) => (
              <Pin
                key={input.id}
                data={input}
                direction='input'
                onConnectionStart={onConnectionStart}
                onConnectionEnd={onConnectionEnd}
                onPinHover={onPinHover}
                onDisruptConnection={onDisruptConnection}
                onValueChange={onPinValueChange}
                isCtrlPressed={isCtrlPressed}
              />
            ))}
        </div>
        <div className='right-col'>
          {data.outputs &&
            data.outputs.map((output) => (
              <Pin
                key={output.id}
                data={output}
                direction='output'
                onConnectionStart={onConnectionStart}
                onConnectionEnd={onConnectionEnd}
                onPinHover={onPinHover}
                onDisruptConnection={onDisruptConnection}
                isCtrlPressed={isCtrlPressed}
              />
            ))}
        </div>
      </div>
    </div>
  );
};
