/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

 // This is where scripts' loading functions should be declared:
void AddSC_assault_on_the_dark_portal();
void AddSC_draenor_shadowmoon_valley();
void AddSC_garrison_generic();

// Auchindoun
void AddSC_instance_auchindoun();
void AddSC_auchindoun();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddDraenorScripts()
{
    AddSC_assault_on_the_dark_portal();
    AddSC_draenor_shadowmoon_valley();
    AddSC_garrison_generic();

    // Auchindoun
    AddSC_instance_auchindoun();
    AddSC_auchindoun();
}
