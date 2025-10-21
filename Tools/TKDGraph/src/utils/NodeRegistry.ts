/** Dependencies */
import type { NodeData } from '../types';
import * as Constants from '../constants';

/**
 * @brief Interface representing a node template.
 */
export interface NodeTemplate {
  id: string;
  name: string;
  category: string;
  description?: string;
  tags?: string[];
  data: NodeData;
  snippet: string;
  public?: boolean;
  once?: boolean;
}

/**
 * @brief Class for managing a registry of node templates.
 * @description Allows registering, unregistering, and retrieving node templates by ID or category.
 */
export class NodeRegistry {
  private templates: Map<string, NodeTemplate> = new Map();
  private categories: Map<string, string[]> = new Map();

  /**
   * @brief Create a new NodeRegistry instance.
   */
  constructor() {
    // Preload constants
    for (const category of Object.keys(Constants)) {
      const templates = (Constants as any)[category] as NodeTemplate[];
      this.registerNodes(templates);
    }
  }

  /**
   * @brief Register a new node template.
   * @param template - The node template to register.
   */
  registerNode(template: NodeTemplate): void {
    if (this.templates.has(template.id)) {
      console.warn(`Node template with id "${template.id}" already exists. Overwriting.`);
    }

    // Set default values
    template.once = template.once ?? false;
    template.public = template.public ?? true;

    // Add to templates map
    this.templates.set(template.id, template);

    // Add to category
    if (!this.categories.has(template.category)) {
      this.categories.set(template.category, []);
    }
    const categoryNodes = this.categories.get(template.category)!;
    if (!categoryNodes.includes(template.id)) {
      categoryNodes.push(template.id);
    }
  }

  /**
   * @brief Register multiple node templates at once
   * @param templates - Array of node templates to register
   */
  registerNodes(templates: NodeTemplate[]): void {
    templates.forEach((template) => this.registerNode(template));
  }

  /**
   * @brief Unregister a node template
   * @param templateId - The ID of the node template to unregister
   * @returns True if the template was successfully unregistered, false otherwise
   */
  unregisterNode(templateId: string): boolean {
    const template = this.templates.get(templateId);
    if (!template) {
      return false;
    }

    // Remove from category
    const categoryNodes = this.categories.get(template.category);
    if (categoryNodes) {
      const index = categoryNodes.indexOf(templateId);
      if (index > -1) {
        categoryNodes.splice(index, 1);
      }
    }

    return this.templates.delete(templateId);
  }

  /**
   * @brief Get a node template by id
   * @param templateId - The ID of the node template to retrieve
   * @returns The node template if found, undefined otherwise
   */
  getTemplate(templateId: string): NodeTemplate | undefined {
    return this.templates.get(templateId);
  }

  /**
   * @brief Get all registered templates
   * @returns An array of all registered node templates
   */
  getAllTemplates(): NodeTemplate[] {
    return Array.from(this.templates.values());
  }

  /**
   * @brief Get templates by category
   * @param category - The category to filter templates by
   * @returns An array of node templates belonging to the specified category
   */
  getTemplatesByCategory(category: string): NodeTemplate[] {
    const nodeIds = this.categories.get(category) || [];
    return nodeIds.map((id) => this.templates.get(id)).filter(Boolean) as NodeTemplate[];
  }

  /**
   * @brief Get all categories
   * @returns An array of all category names
   */
  getCategories(): string[] {
    return Array.from(this.categories.keys());
  }

  /**
   * @brief Search templates by name or tags
   * @param query - The search query
   * @returns An array of node templates matching the search query
   */
  searchTemplates(query: string): NodeTemplate[] {
    const lowerQuery = query.toLowerCase();
    return this.getAllTemplates().filter((template) => {
      const nameMatch = template.name.toLowerCase().includes(lowerQuery);
      const tagMatch = template.tags?.some((tag) => tag.toLowerCase().includes(lowerQuery));
      const categoryMatch = template.category.toLowerCase().includes(lowerQuery);
      return nameMatch || tagMatch || categoryMatch;
    });
  }

  /**
   * @brief Generate an instance Data from a template
   * @param templateId - The ID of the node template
   * @returns A new NodeData instance based on the template, or undefined if the template does not exist
   */
  generateNodeDataFromTemplate(templateId: NodeTemplate): NodeData;
  generateNodeDataFromTemplate(templateId: string | NodeTemplate): NodeData | undefined {
    const template = typeof templateId === 'string' ? this.templates.get(templateId) : templateId;
    if (!template) {
      return undefined;
    }

    // Deep clone the data to avoid mutations
    const clonedData: NodeData = JSON.parse(JSON.stringify(template.data));

    // Assign a unique ID to the node instance
    clonedData.id = this.generateUniqueId();

    // Assign unique IDs to inputs and outputs
    if (clonedData.inputs) {
      clonedData.inputs = clonedData.inputs.map((input) => ({
        ...input,
        id: this.generateUniqueId('input-')
      }));
    }

    if (clonedData.outputs) {
      clonedData.outputs = clonedData.outputs.map((output) => ({
        ...output,
        id: this.generateUniqueId('output-')
      }));
    }

    return clonedData;
  }

  /**
   * @brief Generate a unique ID for a node instance
   * @param prefix - Optional prefix for the ID
   * @returns A unique string ID
   */
  generateUniqueId(prefix: string = 'bue-'): string {
    return `${prefix}${Math.random().toString(36).substring(2, 18)}${Math.random().toString(36).substring(2, 18)}`;
  }

  /**
   * @brief Clear all registered templates
   */
  clear(): void {
    this.templates.clear();
    this.categories.clear();
  }
}
