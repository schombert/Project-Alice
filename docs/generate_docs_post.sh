# Copy all the pngs recursivly to the html folder
find . -type f -not -path "./out/*" -name "*.png" -exec cp {} ./out/html/ \;

# Replace broken code blocks in headers
sed -i "s/&lt;tt&gt;/<code>/g" out/html/md_*.html
sed -i "s/&lt;\/tt&gt;/<\/code>/g" out/html/md_*.html

# Set the about.md to be the start page
mv ./out/html/md_about.html ./out/html/index.html
# Change the link from md_about.hmtl to index.html
sed -i "s/md_about/index/g" ./out/html/index.html

# Fix broken nav tree name in "navtreedata.js"
sed -i 's/\("[^"]*"\),\s*"index\.html"/"Project Alice", "index.html"/' ./out/html/navtreedata.js
