<?xml version="1.0" encoding="UTF-8"?>
<!-- ============================================================ -->
<!-- IMAGES.gml                              -->
<!-- ============================================================ -->
<!-- ============================================================ -->
<!-- NOTE: You can optionally associate an "<idname>" element to  -->
<!-- any of the widgets.                                          -->
<!-- The "<idname>" element is required if the application        -->
<!-- business part (written in "C" code) needs to interact with   -->
<!-- the widget.                                                  -->
<!-- The value inside the "<idname>" element must be one of the   -->
<!-- "idname" attributes declared in the "widgetMappings.iml"     -->
<!-- file (located at project root). Please complete this file    -->
<!-- if needed.                                                   -->
<!-- The uniqueness of each <idname> is automatically checked     -->
<!-- (in the editor and at build time).                           -->
<!-- ============================================================ -->
<DAL xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:noNamespaceSchemaLocation="http://www.ingenico.com/schemas/dal/1_0">

  <version DAL_version="01.00" this_version="01.00" />

  <!-- TO BE CUSTOMIZED BY THE DEVELOPER -->

  <!-- Declare a new GOAL screen (this name is used when calling -->
  <!-- 'GL_GraphicLib_ReadResource' in order to render this      -->
  <!-- resource file).                                           -->
  <goal name="IMAGES">
    <!-- Create a window -->
    <window>
      <!-- Add children to window -->
      <children>
        <!-- Add label to children -->
        <label>
          <!-- Set the label text -->
          <text>This is an empty screen!</text>
        </label>
      </children>
    </window>
  </goal>
</DAL>
