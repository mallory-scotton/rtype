/** Dependencies */
import { NodeRegistry } from '../utils';
import { createContext, useContext, useState, useEffect, useRef } from 'react';
import type { ReactNode } from 'react';
import type { BlueprintData, MultiSelectBox, CanvasTransform, NodeEntry, PinDirection } from '../types';

const STORAGE_KEY = 'tkd-graph-blueprints';
const STORAGE_INDEX_KEY = 'tkd-graph-current-index';

/**
 * @brief Connection state for drawing connections between pins
 */
export interface ConnectingFromState {
  pinId: string;
  direction: PinDirection;
  pinType: string;
  element: HTMLElement;
}

/**
 * @brief Editor context type
 */
interface EditorContextType {
  nodeRegistry: NodeRegistry;
  classList: string[];
  blueprints: BlueprintData[];
  currentBlueprintIndex: number;
  multiSelectBox: MultiSelectBox | null;
  canvasRef: React.RefObject<HTMLDivElement | null>;
  canvasTransform: CanvasTransform;
  selectedNodeIds: string[];
  selectedConnectionId: string | null;
  hoveredConnectionId: string | null;
  isDrawingMultiSelect: React.RefObject<boolean>;
  multiSelectStartNodeIds: React.RefObject<string[]>;
  isPanning: boolean;
  connectingFrom: ConnectingFromState | null;
  hoveredPin: { pinId: string; direction: PinDirection; pinType: string } | null;
  mousePosition: { x: number; y: number } | null;
  isCtrlPressed: boolean;
  isAltPressed: boolean;
  connectionCompletedRef: React.RefObject<boolean>;
  connectionUpdateTrigger: number;
  triggerConnectionUpdate: () => void;
  setBlueprints: (blueprints: BlueprintData[]) => void;
  setCurrentBlueprintIndex: (index: number) => void;
  addBlueprint: (blueprint: BlueprintData) => void;
  removeBlueprint: (index: number) => void;
  setMultiSelectBox: (box: MultiSelectBox | null) => void;
  setCanvasTransform: (transform: CanvasTransform) => void;
  setSelectedNodeIds: (ids: string[]) => void;
  setSelectedConnectionId: (id: string | null) => void;
  setHoveredConnectionId: (id: string | null) => void;
  setIsPanning: (isPanning: boolean) => void;
  setConnectingFrom: (state: ConnectingFromState | null) => void;
  setHoveredPin: (pin: { pinId: string; direction: PinDirection; pinType: string } | null) => void;
  setMousePosition: (pos: { x: number; y: number } | null) => void;
  setIsCtrlPressed: (pressed: boolean) => void;
  addNodeToBlueprint: (nodeData: NodeEntry, blueprintIndex?: number) => void;
  removeNodeFromBlueprint: (nodeId: string, blueprintIndex?: number) => void;
  removeSelectedNodesFromCurrentBlueprint: () => void;
  createConnection: (sourcePinId: string, targetPinId: string, sourceNodeId: string, targetNodeId: string) => void;
  deleteConnection: (connectionId: string) => void;
  updatePinValue: (pinId: string, value: any) => void;
}

/**
 * @brief Editor context react hook
 */
export const EditorContext = createContext<EditorContextType | undefined>(undefined);

/**
 * @brief Editor context provider props
 */
interface EditorProviderProps {
  children: ReactNode;
}

/**
 * @brief Editor context provider component
 * @description Provides editor state and functions to all child components
 */
export function EditorProvider({ children }: EditorProviderProps) {
  const nodeRegistry = new NodeRegistry();
  const classList: string[] = [];
  const [canvasTransform, setCanvasTransform] = useState<CanvasTransform>({ translateX: 0, translateY: 0, scale: 1 });
  const [selectedNodeIds, setSelectedNodeIds] = useState<string[]>([]);
  const [selectedConnectionId, setSelectedConnectionId] = useState<string | null>(null);
  const [hoveredConnectionId, setHoveredConnectionId] = useState<string | null>(null);
  const isDrawingMultiSelect = useRef(false);
  const canvasRef = useRef<HTMLDivElement | null>(null);
  const multiSelectStartNodeIds = useRef<string[]>([]);
  const [isPanning, setIsPanning] = useState(false);
  const connectionCompletedRef = useRef(false);

  // Connection-related state
  const [connectingFrom, setConnectingFrom] = useState<ConnectingFromState | null>(null);
  const [hoveredPin, setHoveredPin] = useState<{ pinId: string; direction: PinDirection; pinType: string } | null>(
    null
  );
  const [mousePosition, setMousePosition] = useState<{ x: number; y: number } | null>(null);
  const [isCtrlPressed, setIsCtrlPressed] = useState(false);
  const [isAltPressed, setIsAltPressed] = useState(false);
  const [connectionUpdateTrigger, setConnectionUpdateTrigger] = useState(0);

  const triggerConnectionUpdate = () => {
    setConnectionUpdateTrigger((prev) => prev + 1);
  };

  // Load blueprints from localStorage on initial mount
  const [blueprints, setBlueprints] = useState<BlueprintData[]>(() => {
    try {
      const stored = localStorage.getItem(STORAGE_KEY);
      if (stored) {
        const parsed = JSON.parse(stored);
        return parsed;
      }
    } catch (error) {
      console.error('Failed to load blueprints from localStorage:', error);
    }
    return [];
  });

  const [currentBlueprintIndex, setCurrentBlueprintIndex] = useState<number>(() => {
    try {
      const stored = localStorage.getItem(STORAGE_INDEX_KEY);
      if (stored) {
        return parseInt(stored, 10);
      }
    } catch (error) {
      console.error('Failed to load current index from localStorage:', error);
    }
    return -1;
  });

  // Multi-select box state
  const [multiSelectBox, setMultiSelectBox] = useState<MultiSelectBox | null>(null);

  const addBlueprint = (blueprint: BlueprintData) => {
    setBlueprints((prev) => {
      const newBlueprints = [...prev, blueprint];
      setCurrentBlueprintIndex(newBlueprints.length - 1);
      return newBlueprints;
    });
  };

  const removeBlueprint = (index: number) => {
    setBlueprints((prev) => prev.filter((_, i) => i !== index));
    setCurrentBlueprintIndex(-1);
  };

  const addNodeToBlueprint = (nodeData: NodeEntry, blueprintIndex: number = currentBlueprintIndex) => {
    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      newBlueprints[blueprintIndex].nodes.push(nodeData);
      return newBlueprints;
    });
  };

  const removeNodeFromBlueprint = (nodeId: string, blueprintIndex: number = currentBlueprintIndex) => {
    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      newBlueprints[blueprintIndex].nodes = newBlueprints[blueprintIndex].nodes.filter(
        (node) => node.data.id !== nodeId
      );
      return newBlueprints;
    });
  };

  // Auto-save blueprints to localStorage whenever they change
  useEffect(() => {
    try {
      if (blueprints.length > 0) {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(blueprints));
      } else {
        localStorage.removeItem(STORAGE_KEY);
      }
    } catch (error) {
      console.error('Failed to save blueprints to localStorage:', error);
    }
  }, [blueprints]);

  // Auto-save current blueprint index
  useEffect(() => {
    try {
      if (currentBlueprintIndex >= 0) {
        localStorage.setItem(STORAGE_INDEX_KEY, currentBlueprintIndex.toString());
      } else {
        localStorage.removeItem(STORAGE_INDEX_KEY);
      }
    } catch (error) {
      console.error('Failed to save current index to localStorage:', error);
    }
  }, [currentBlueprintIndex]);

  // Track Ctrl key state for disrupting connections
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.ctrlKey || event.metaKey) {
        setIsCtrlPressed(true);
      }
      if (event.altKey) {
        setIsAltPressed(true);
      }
    };

    const handleKeyUp = (event: KeyboardEvent) => {
      if (!event.ctrlKey && !event.metaKey) {
        setIsCtrlPressed(false);
      }
      if (!event.altKey) {
        setIsAltPressed(false);
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, []);

  // Handle global mouseup to end connections when not hovering a pin
  useEffect(() => {
    if (!connectingFrom) return;

    const handleGlobalMouseUp = () => {
      // Small delay to let pin mouseup handlers run first
      setTimeout(() => {
        // If connection wasn't completed by a pin handler, cancel it
        if (!connectionCompletedRef.current) {
          setConnectingFrom(null);
          setMousePosition(null);
          setHoveredPin(null);
        }
        // Reset the flag for next connection
        connectionCompletedRef.current = false;
      }, 10);
    };

    window.addEventListener('mouseup', handleGlobalMouseUp);

    return () => {
      window.removeEventListener('mouseup', handleGlobalMouseUp);
    };
  }, [connectingFrom]);

  const removeSelectedNodesFromCurrentBlueprint = () => {
    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      newBlueprints[currentBlueprintIndex].connections = newBlueprints[currentBlueprintIndex].connections.filter(
        (conn) => !selectedNodeIds.includes(conn.sourceNodeId) && !selectedNodeIds.includes(conn.targetNodeId)
      );
      newBlueprints[currentBlueprintIndex].nodes = newBlueprints[currentBlueprintIndex].nodes.filter(
        (node) => !selectedNodeIds.includes(node.data.id)
      );
      return newBlueprints;
    });
    setSelectedNodeIds([]);
  };

  const createConnection = (sourcePinId: string, targetPinId: string, sourceNodeId: string, targetNodeId: string) => {
    if (currentBlueprintIndex < 0) return;

    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      const currentBlueprint = newBlueprints[currentBlueprintIndex];

      // Remove any existing connection to the target pin (input can only have one connection)
      const connectionsWithoutTarget = currentBlueprint.connections.filter((conn) => conn.targetPinId !== targetPinId);

      // Create new connection
      const newConnection = {
        id: `${sourcePinId},${targetPinId}`,
        sourcePinId,
        targetPinId,
        sourceNodeId,
        targetNodeId
      };

      newBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        connections: [...connectionsWithoutTarget, newConnection]
      };

      return newBlueprints;
    });
  };

  const deleteConnection = (connectionId: string) => {
    if (currentBlueprintIndex < 0) return;

    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      const currentBlueprint = newBlueprints[currentBlueprintIndex];

      newBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        connections: currentBlueprint.connections.filter((conn) => conn.id !== connectionId)
      };

      return newBlueprints;
    });

    if (selectedConnectionId === connectionId) {
      setSelectedConnectionId(null);
    }
  };

  const updatePinValue = (pinId: string, value: any) => {
    if (currentBlueprintIndex < 0) return;

    setBlueprints((prev) => {
      const newBlueprints = [...prev];
      const currentBlueprint = newBlueprints[currentBlueprintIndex];

      const updatedNodes = currentBlueprint.nodes.map((node) => {
        const updatedInputs = node.data.inputs?.map((pin) => {
          if (pin.id === pinId) {
            return { ...pin, value };
          }
          return pin;
        });

        return {
          ...node,
          data: {
            ...node.data,
            inputs: updatedInputs
          }
        };
      });

      newBlueprints[currentBlueprintIndex] = {
        ...currentBlueprint,
        nodes: updatedNodes
      };

      return newBlueprints;
    });
  };

  //! DEBUG ONLY
  //! Ensure at least one blueprint exists
  if (blueprints.length === 0) {
    setBlueprints([
      {
        className: 'BP_Player',
        type: 'APawn',
        nodes: [],
        connections: [],
        properties: [],
        functions: []
      }
    ]);
    setCurrentBlueprintIndex(0);
  }

  const contextValue: EditorContextType = {
    nodeRegistry,
    classList,
    blueprints,
    currentBlueprintIndex,
    multiSelectBox,
    canvasRef,
    canvasTransform,
    selectedNodeIds,
    selectedConnectionId,
    hoveredConnectionId,
    isDrawingMultiSelect,
    multiSelectStartNodeIds,
    isPanning,
    connectingFrom,
    hoveredPin,
    mousePosition,
    isCtrlPressed,
    isAltPressed,
    connectionCompletedRef,
    connectionUpdateTrigger,
    triggerConnectionUpdate,
    setBlueprints,
    setCurrentBlueprintIndex,
    addBlueprint,
    removeBlueprint,
    setMultiSelectBox,
    setCanvasTransform,
    setSelectedNodeIds,
    setSelectedConnectionId,
    setHoveredConnectionId,
    setIsPanning,
    setConnectingFrom,
    setHoveredPin,
    setMousePosition,
    setIsCtrlPressed,
    addNodeToBlueprint,
    removeNodeFromBlueprint,
    removeSelectedNodesFromCurrentBlueprint,
    createConnection,
    deleteConnection,
    updatePinValue
  };

  return <EditorContext.Provider value={contextValue}>{children}</EditorContext.Provider>;
}

/**
 * @brief Hook to use the Editor context
 * @description Custom hook that provides access to the editor context
 * @throws Error if used outside of EditorProvider
 */
export function useEditor() {
  const context = useContext(EditorContext);
  if (context === undefined) {
    throw new Error('useEditor must be used within an EditorProvider');
  }
  return context;
}
