# Scripting changes by SneakBug8

- Moved links to triggers & effects away from UI element definition into a special `scripted_interaction` DCON table. This reduces memory usage (there is over 8k UI elements in the basegame each having 6 bytes for these links) and increases calculations during UI update (find the interaction ID from GUI element ID).
- AI can now use scripted buttons that have `ai_will_do` evaluation defined by iterating over a limited number of scripted interactions along with decisions.
