var labels = [];
[].slice.call(document.querySelectorAll('.js-label-link'))
    .forEach(function(element) {
      labels.push({
        name: element.textContent.trim(),
        description: element.getAttribute('title'),
        // using style.backgroundColor might returns "rgb(...)"
        color: element.getAttribute('style')
                   .replace('background-color:', '')
                   .replace(/color:.*/, '')
                   .trim()
                   // github wants hex code only without # or ;
                   .replace(/^#/, '')
                   .replace(/;$/, '')
                   .trim(),
      })
    })

console.log(JSON.stringify(labels, null, 2))
