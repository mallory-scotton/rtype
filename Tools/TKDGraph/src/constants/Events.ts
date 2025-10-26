/** Dependencies */
import type { NodeIconType } from '../types';
import type { NodeTemplate } from '../utils';

/**
 * @brief Generates key event node templates.
 * @param key The key for which to generate the event node.
 * @returns A NodeTemplate representing the key event.
 */
function generateKeyEvents(key: string, icon: NodeIconType, category: string): NodeTemplate {
  return {
    id: `event_key_${key}`,
    name: `Event Key (${key})`,
    category: `Events>${category}`,
    description: `Called when a key of type ${key} is actioned.`,
    tags: ['event', 'key', 'released', 'pressed', key],
    data: {
      id: '',
      type: 'event',
      header: {
        label: key,
        type: 'event',
        icon: icon
      },
      inputs: [],
      outputs: [
        { id: '', type: 'exec', label: 'Pressed' },
        { id: '', type: 'exec', label: 'Released' },
        { id: '', type: 'struct', label: 'Key' }
      ]
    },
    // TODO: Add snippet
    snippet: ``
  };
}

/**
 * @brief List of event node templates.
 */
export const Events: NodeTemplate[] = [
  {
    id: 'event_begin_play',
    name: 'Event BeginPlay',
    category: 'Events',
    description: 'Called when the game starts or when spawned.',
    tags: ['event', 'begin play', 'start'],
    data: {
      id: '',
      type: 'event',
      header: {
        label: 'Event BeginPlay',
        type: 'event',
        icon: 'event'
      },
      inputs: [],
      outputs: [{ id: '', type: 'exec' }],
      label: 'Begin Play'
    },
    snippet: `{OUTPUT_1}`,
    once: true
  },
  {
    id: 'event_end_play',
    name: 'Event EndPlay',
    category: 'Events',
    description: 'Called when the game ends or when destroyed.',
    tags: ['event', 'end play', 'stop'],
    data: {
      id: '',
      type: 'event',
      header: {
        label: 'Event EndPlay',
        type: 'event',
        icon: 'event'
      },
      inputs: [],
      outputs: [{ id: '', type: 'exec' }],
      label: 'End Play'
    },
    snippet: `{OUTPUT_1}`,
    once: true
  },
  {
    id: 'event_tick',
    name: 'Event Tick',
    category: 'Events',
    description: 'Called every frame while the game is running.',
    tags: ['event', 'tick', 'update'],
    data: {
      id: '',
      type: 'event',
      header: {
        label: 'Event Tick',
        type: 'event',
        icon: 'event'
      },
      inputs: [],
      outputs: [
        { id: '', type: 'exec' },
        { id: '', type: 'float', label: 'Delta Seconds' }
      ],
      label: 'Tick'
    },
    snippet: `{OUTPUT_1}`,
    once: true
  },
  ...[
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    'F1',
    'F2',
    'F3',
    'F4',
    'F5',
    'F6',
    'F7',
    'F8',
    'F9',
    'F10',
    'F11',
    'F12',
    'Up',
    'Down',
    'Left',
    'Right',
    'SpaceBar',
    'Enter',
    'LeftShift',
    'RightShift',
    'LeftCtrl',
    'RightCtrl',
    'LeftAlt',
    'RightAlt',
    'Tab',
    'Escape',
    'Backspace',
    'Insert',
    'Delete',
    'Home',
    'End',
    'PageUp',
    'PageDown',
    'CapsLock',
    'NumLock',
    'Num0',
    'Num1',
    'Num2',
    'Num3',
    'Num4',
    'Num5',
    'Num6',
    'Num7',
    'Num8',
    'Num9',
    'NumpadDivide',
    'NumpadMultiply',
    'NumpadSubtract',
    'NumpadAdd',
    'NumpadEnter',
    'NumpadDecimal',
    'Numpad0',
    'Numpad1',
    'Numpad2',
    'Numpad3',
    'Numpad4',
    'Numpad5',
    'Numpad6',
    'Numpad7',
    'Numpad8',
    'Numpad9'
  ].map((key) => generateKeyEvents(key, 'input-key', 'Keyboard')),
  ...['LeftMouseButton', 'RightMouseButton', 'MiddleMouseButton'].map((key) =>
    generateKeyEvents(key, 'input-mouse', 'Mouse')
  )
];
