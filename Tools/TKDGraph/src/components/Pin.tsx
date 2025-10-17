/** Dependencies */
import type { PinData, PinDirection } from '../types';
import React from 'react';

/**
 * @brief Interface for Pin component props
 * @description This interface defines the structure of the props that the Pin component expects to receive.
 */
interface PinProps {
  data: PinData;
  direction: PinDirection;
  onConnectionStart?: (pinId: string, direction: PinDirection, pinType: string) => void;
  onConnectionEnd?: (pinId: string, direction: PinDirection, pinType: string) => void;
  onPinHover?: (pinId: string, direction: PinDirection, pinType: string, isHovering: boolean) => void;
  onDisruptConnection?: (pinId: string, direction: PinDirection, pinType: string) => void;
  isCtrlPressed?: boolean;
}

/**
 * @brief Pin component
 * @description This component represents a pin in a node, displaying its data.
 */
export const Pin: React.FC<PinProps> = ({
  data,
  direction,
  onConnectionStart,
  onConnectionEnd,
  onPinHover,
  onDisruptConnection,
  isCtrlPressed = false
}) => {
  const handleMouseDown = (event: React.MouseEvent) => {
    event.stopPropagation();
    event.preventDefault();

    // If CTRL is pressed and pin is filled (connected), disrupt the connection
    if (isCtrlPressed && data.filled && onDisruptConnection) {
      onDisruptConnection(data.id, direction, data.type);
    } else {
      onConnectionStart?.(data.id, direction, data.type);
    }
  };

  const handleMouseUp = (event: React.MouseEvent) => {
    event.stopPropagation();
    event.preventDefault();
    onConnectionEnd?.(data.id, direction, data.type);
  };

  const handleMouseEnter = () => {
    onPinHover?.(data.id, direction, data.type, true);
  };

  const handleMouseLeave = () => {
    onPinHover?.(data.id, direction, data.type, false);
  };

  return (
    <div
      key={data.id}
      className={`pin${data.noMarginBottom ? ' no-margin-bottom' : ''}`}
      data-id={data.id}
      data-type={data.type}
      data-direction={direction}
    >
      <div
        className={`div-inside ${data.type}`}
        onMouseDown={handleMouseDown}
        onMouseUp={handleMouseUp}
        onMouseEnter={handleMouseEnter}
        onMouseLeave={handleMouseLeave}
      >
        {direction === 'input' && (
          <>
            <div
              className={`clink ${data.type === 'exec' ? 'connector-image' : 'connector'} ${data.type} ${
                data.filled ? 'filled' : ''
              }`}
            ></div>
            <div className='label-text'>{data.label ?? ''}</div>
          </>
        )}
        {direction === 'output' && (
          <>
            <div className='label-text'>{data.label ?? ''}</div>
            <div
              className={`clink ${data.type === 'exec' ? 'connector-image' : 'connector'} ${data.type} ${
                data.filled ? 'filled' : ''
              }`}
            ></div>
          </>
        )}
      </div>
    </div>
  );
};
