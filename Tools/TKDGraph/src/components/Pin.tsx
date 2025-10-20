/** Dependencies */
import type { PinData, PinDirection } from '../types';
import React, { useState, useEffect } from 'react';
import { canPinTypeHaveInput, convertPinTypeToDefaultValue } from '../utils/Convert';

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
  onValueChange?: (pinId: string, value: any) => void;
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
  onValueChange,
  isCtrlPressed = false
}) => {
  const [inputValue, setInputValue] = useState<any>(data.value ?? convertPinTypeToDefaultValue(data.type));

  useEffect(() => {
    setInputValue(data.value ?? convertPinTypeToDefaultValue(data.type));
  }, [data.value, data.type]);

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

  const handleInputChange = (newValue: any) => {
    setInputValue(newValue);
    onValueChange?.(data.id, newValue);
  };

  const handleInputClick = (event: React.MouseEvent) => {
    event.stopPropagation();
  };

  const handleInputMouseDown = (event: React.MouseEvent) => {
    event.stopPropagation();
  };

  const handleKeyDown = (event: React.KeyboardEvent<HTMLSpanElement>) => {
    // Allow only valid characters based on pin type
    if (data.type === 'int' || data.type === 'byte') {
      if (
        !/[0-9\-]/.test(event.key) &&
        event.key !== 'Backspace' &&
        event.key !== 'Delete' &&
        event.key !== 'ArrowLeft' &&
        event.key !== 'ArrowRight'
      ) {
        event.preventDefault();
      }
    } else if (data.type === 'float' || data.type === 'real') {
      if (
        !/[0-9\-.]/.test(event.key) &&
        event.key !== 'Backspace' &&
        event.key !== 'Delete' &&
        event.key !== 'ArrowLeft' &&
        event.key !== 'ArrowRight'
      ) {
        event.preventDefault();
      }
    }
  };

  const handleInputBlur = (event: React.FocusEvent<HTMLSpanElement>) => {
    const text = event.currentTarget.textContent || '';
    let newValue: any;

    if (data.type === 'int' || data.type === 'byte') {
      newValue = parseInt(text) || 0;
    } else if (data.type === 'float' || data.type === 'real') {
      newValue = parseFloat(text) || 0.0;
    } else {
      newValue = text;
    }

    handleInputChange(newValue);
  };

  const renderInputField = () => {
    if (!canPinTypeHaveInput(data.type)) {
      return null;
    }

    if (data.type === 'bool') {
      return (
        <input
          type='checkbox'
          checked={inputValue}
          onChange={(e) => handleInputChange(e.target.checked)}
          onClick={handleInputClick}
          onMouseDown={handleInputMouseDown}
        />
      );
    }

    // For numeric and text types, use fake-input span
    return (
      <span
        className='fake-input'
        contentEditable
        suppressContentEditableWarning
        onBlur={handleInputBlur}
        onKeyDown={handleKeyDown}
        onClick={handleInputClick}
        onMouseDown={handleInputMouseDown}
      >
        {inputValue}
      </span>
    );
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
            {!data.filled && renderInputField()}
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
