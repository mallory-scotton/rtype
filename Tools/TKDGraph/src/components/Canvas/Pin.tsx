/** Dependencies */
import type { PinData, PinDirection } from '../../types';
import React, { useState, useEffect } from 'react';
import { canPinTypeHaveInput, convertPinTypeToDefaultValue } from '../../utils/Convert';
import { useEditor } from '../../context';
import './Pin.css';

/**
 * @brief Interface for Pin component props
 * @description This interface defines the structure of the props that the Pin component expects to receive.
 */
interface PinProps {
  data: PinData;
  direction: PinDirection;
  nodeId: string;
}

/**
 * @brief Pin component
 * @description This component represents a pin in a node, displaying its data and handling connections.
 */
export const Pin: React.FC<PinProps> = ({ data, direction, nodeId: _nodeId }) => {
  const {
    connectingFrom,
    setConnectingFrom,
    hoveredPin: _hoveredPin,
    setHoveredPin,
    setMousePosition,
    isCtrlPressed,
    blueprints,
    currentBlueprintIndex,
    createConnection,
    deleteConnection,
    updatePinValue,
    canvasRef,
    connectionCompletedRef
  } = useEditor();

  const [inputValue, setInputValue] = useState<any>(data.value ?? convertPinTypeToDefaultValue(data.type));

  useEffect(() => {
    setInputValue(data.value ?? convertPinTypeToDefaultValue(data.type));
  }, [data.value, data.type]);

  const handleMouseDown = (event: React.MouseEvent) => {
    event.stopPropagation();
    event.preventDefault();

    // If CTRL is pressed and pin is filled (connected), disrupt the connection
    if (isCtrlPressed && data.filled) {
      handleDisruptConnection();
    } else {
      handleConnectionStart(event);
    }
  };

  const handleMouseUp = (event: React.MouseEvent) => {
    event.stopPropagation();
    event.preventDefault();
    handleConnectionEnd();
  };

  const handleMouseEnter = () => {
    setHoveredPin({ pinId: data.id, direction, pinType: data.type });
  };

  const handleMouseLeave = () => {
    setHoveredPin(null);
  };

  const handleConnectionStart = (event: React.MouseEvent) => {
    if (!canvasRef.current) return;

    const pinElement = canvasRef.current.querySelector(`.pin[data-id="${data.id}"] .clink`) as HTMLElement;

    if (pinElement) {
      setConnectingFrom({
        pinId: data.id,
        direction,
        pinType: data.type,
        element: pinElement
      });

      // Set initial mouse position from the actual mouse event
      setMousePosition({ x: event.clientX, y: event.clientY });
    }
  };

  const handleConnectionEnd = () => {
    if (!connectingFrom || currentBlueprintIndex < 0) {
      setMousePosition(null);
      return;
    }

    // Mark that we're handling the connection
    connectionCompletedRef.current = true;

    // Reset mouse position
    setMousePosition(null);

    // Prevent connecting same direction pins or same pin
    if (connectingFrom.direction === direction || connectingFrom.pinId === data.id) {
      setConnectingFrom(null);
      return;
    }

    // Prevent connecting different pin types - types must match exactly
    if (connectingFrom.pinType !== data.type) {
      console.warn(`Cannot connect ${connectingFrom.pinType} to ${data.type}. Pin types must match.`);
      setConnectingFrom(null);
      return;
    }

    // Find the nodes that contain these pins
    const currentBlueprint = blueprints[currentBlueprintIndex];
    let sourceNodeId = '';
    let targetNodeId = '';
    let sourcePinId = '';
    let targetPinId = '';

    // Output pins should be source, input pins should be target
    if (direction === 'input') {
      sourcePinId = connectingFrom.pinId;
      targetPinId = data.id;
    } else {
      sourcePinId = data.id;
      targetPinId = connectingFrom.pinId;
    }

    // Find node IDs
    for (const node of currentBlueprint.nodes) {
      const allPins = [...(node.data.inputs || []), ...(node.data.outputs || [])];
      if (allPins.some((p) => p.id === sourcePinId)) {
        sourceNodeId = node.data.id;
      }
      if (allPins.some((p) => p.id === targetPinId)) {
        targetNodeId = node.data.id;
      }
    }

    if (!sourceNodeId || !targetNodeId) {
      setConnectingFrom(null);
      return;
    }

    // Create the connection
    createConnection(sourcePinId, targetPinId, sourceNodeId, targetNodeId);
    setConnectingFrom(null);
  };

  const handleDisruptConnection = () => {
    if (currentBlueprintIndex < 0 || !canvasRef.current) return;

    const currentBlueprint = blueprints[currentBlueprintIndex];

    // Find and remove the connection involving this pin
    const connectionToRemove = currentBlueprint.connections.find(
      (conn) => conn.sourcePinId === data.id || conn.targetPinId === data.id
    );

    if (connectionToRemove) {
      deleteConnection(connectionToRemove.id);

      // Start a new connection from the CLICKED pin (keep the other end)
      // User wants to reconnect this pin to something else, keeping the other pin in place
      const pinElement = canvasRef.current.querySelector(`.pin[data-id="${data.id}"] .clink`) as HTMLElement;
      if (pinElement) {
        setConnectingFrom({
          pinId: data.id,
          direction: direction,
          pinType: data.type,
          element: pinElement
        });

        // Set initial mouse position for smooth dragging
        setMousePosition({
          x: window.event ? (window.event as MouseEvent).clientX : 0,
          y: window.event ? (window.event as MouseEvent).clientY : 0
        });
      }
    }
  };

  const handleInputChange = (newValue: any) => {
    setInputValue(newValue);
    updatePinValue(data.id, newValue);
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

    if (data.type === 'object') {
      return (
        <span className='fake-input' contentEditable suppressContentEditableWarning>
          {inputValue}
        </span>
      );
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
            {!data.filled && !data.hideInput && renderInputField()}
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
