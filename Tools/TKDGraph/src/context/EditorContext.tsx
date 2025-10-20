/** Dependencies */
import { NodeRegistry } from '../utils';
import { createContext, useContext, useState, useEffect } from 'react';
import type { ReactNode } from 'react';
import type { BlueprintData } from '../types';

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
  setBlueprints: (blueprints: BlueprintData[]) => void;
  setCurrentBlueprintIndex: (index: number) => void;
  addBlueprint: (blueprint: BlueprintData) => void;
  removeBlueprint: (index: number) => void;
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

  const contextValue: EditorContextType = {
    nodeRegistry,
    classList,
    blueprints,
    currentBlueprintIndex,
    setBlueprints,
    setCurrentBlueprintIndex,
    addBlueprint,
    removeBlueprint
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
