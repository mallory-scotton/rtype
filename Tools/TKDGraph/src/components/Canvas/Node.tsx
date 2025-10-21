/** Dependencies */
import type { NodeData, NodeEntry, PinDirection } from '../../types';
import { Pin } from './Pin';
import { useState, useRef, useEffect } from 'react';
import { snapToGrid } from '../../utils';
import './Node.css';
import { useEditor } from '../../context';

/**
 * @brief Interface for Node component props
 * @description This interface defines the structure of the props that the Node component expects to receive.
 */
interface NodeProps {
  entry: NodeEntry;
}

/**
 * @brief Node component
 * @description This component represents a node in a graph, displaying its data.
 */
export const Node: React.FC<NodeProps> = ({ entry }) => {
  const { selectedNodeIds } = useEditor();

  const classes: string[] = ['node'];
  const [isDragging, _] = useState(false);

  const { data, position, dimensions } = entry;

  let allPins = [...(data.inputs || []), ...(data.outputs || [])];
  let nodeType = allPins.find((pin) => pin.type !== 'exec')?.type || '';

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

  if (selectedNodeIds.some((id) => id === entry.data.id)) {
    classes.push('selected');
  }

  return (
    <div
      data-id={data.id}
      style={{
        position: 'absolute',
        transform: `translate(${position.x}px, ${position.y}px)`,
        width: dimensions?.width,
        height: dimensions?.height,
        backgroundColor: data.backgroundColor,
        cursor: isDragging ? 'grabbing' : 'grab'
      }}
      className={classes.join(' ')}
      // onMouseDown={handleMouseDown}
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
                // onConnectionStart={onConnectionStart}
                // onConnectionEnd={onConnectionEnd}
                // onPinHover={onPinHover}
                // onDisruptConnection={onDisruptConnection}
                // onValueChange={onPinValueChange}
                // isCtrlPressed={isCtrlPressed}
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
                // onConnectionStart={onConnectionStart}
                // onConnectionEnd={onConnectionEnd}
                // onPinHover={onPinHover}
                // onDisruptConnection={onDisruptConnection}
                // isCtrlPressed={isCtrlPressed}
              />
            ))}
        </div>
      </div>
    </div>
  );
};
