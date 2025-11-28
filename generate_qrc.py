import os

print('<RCC>')
print('    <qresource prefix="/">')

for root, dirs, files in os.walk('source'):
    for file in files:
        # Use forward slashes for Qt resources
        path = os.path.join(root, file).replace(os.sep, '/')
        print(f'        <file>{path}</file>')

print('    </qresource>')
print('</RCC>')
