/**
 * @brief Connection Interface
 * @description Represents a connection between two pins in the blueprint
 */
export interface Connection {
  id: string;
  sourcePinId: string;
  targetPinId: string;
  sourceNodeId: string;
  targetNodeId: string;
}

/**
 * @brief Connection Data for rendering
 * @description Contains the computed SVG path data for rendering a connection
 */
export interface ConnectionRenderData {
  connection: Connection;
  path: string;
  color: string;
  x: number;
  y: number;
  width: number;
  height: number;
}
