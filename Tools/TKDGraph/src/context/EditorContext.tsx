/** Dependencies */
import { NodeRegistry } from '../utils';
import { createContext, useContext, useState, useEffect, useRef } from 'react';
import type { ReactNode } from 'react';
import type { BlueprintData, MultiSelectBox, CanvasTransform, NodeEntry } from '../types';

const STORAGE_KEY = 'tkd-graph-blueprints';
const STORAGE_INDEX_KEY = 'tkd-graph-current-index';

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
  isDrawingMultiSelect: React.RefObject<boolean>;
  multiSelectStartNodeIds: React.RefObject<string[]>;
  isPanning: boolean;
  setBlueprints: (blueprints: BlueprintData[]) => void;
  setCurrentBlueprintIndex: (index: number) => void;
  addBlueprint: (blueprint: BlueprintData) => void;
  removeBlueprint: (index: number) => void;
  setMultiSelectBox: (box: MultiSelectBox | null) => void;
  setCanvasTransform: (transform: CanvasTransform) => void;
  setSelectedNodeIds: (ids: string[]) => void;
  setIsPanning: (isPanning: boolean) => void;
  addNodeToBlueprint: (nodeData: NodeEntry, blueprintIndex?: number) => void;
  removeNodeFromBlueprint: (nodeId: string, blueprintIndex?: number) => void;
  removeSelectedNodesFromCurrentBlueprint: () => void;
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
  const isDrawingMultiSelect = useRef(false);
  const canvasRef = useRef<HTMLDivElement | null>(null);
  const multiSelectStartNodeIds = useRef<string[]>([]);
  const [isPanning, setIsPanning] = useState(false);

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
    isDrawingMultiSelect,
    multiSelectStartNodeIds,
    isPanning,
    setBlueprints,
    setCurrentBlueprintIndex,
    addBlueprint,
    removeBlueprint,
    setMultiSelectBox,
    setCanvasTransform,
    setSelectedNodeIds,
    setIsPanning,
    addNodeToBlueprint,
    removeNodeFromBlueprint,
    removeSelectedNodesFromCurrentBlueprint
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
