#!/bin/bash
# Test script for GUI tools with detailed debugging

echo "Testing GUI Tools on macOS"
echo "========================="

# Set up environment
export QT_PLUGIN_PATH=/opt/homebrew/opt/qt@5/plugins
export QT_MAC_WANTS_LAYER=1
export QT_LOGGING_RULES="qt.qpa.*=true"

# Create test data
echo "Creating test data..."
cat > /tmp/guilogger_test.txt << EOF
#C test_channel[m]
0.5
0.7
0.3
#RESET
EOF

cat > /tmp/matrixviz_test.txt << EOF
#M testmatrix 2 2
1.0 2.0
3.0 4.0
#M testmatrix 2 2
1.5 2.5
3.5 4.5
EOF

echo ""
echo "1. Testing guilogger..."
echo "Command: guilogger -m file -f /tmp/guilogger_test.txt"
echo "Note: This should open a GUI window showing a plot"
echo "Press Ctrl+C to continue after viewing"
/Users/jerry/lpzrobots_mac/guilogger/src/bin/guilogger.app/Contents/MacOS/guilogger -m file -f /tmp/guilogger_test.txt

echo ""
echo "2. Testing matrixviz..."
echo "Command: cat /tmp/matrixviz_test.txt | matrixviz -novideo"
echo "Note: This should open a GUI window showing matrix visualization"
echo "Press Ctrl+C to continue after viewing"
cat /tmp/matrixviz_test.txt | /Users/jerry/lpzrobots_mac/matrixviz/bin/matrixviz -novideo

echo ""
echo "Test completed!"