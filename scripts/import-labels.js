// Copy/paste the following in the console in labels

[{
  'name': '01 - type: bug',
  'description': 'Something isn\'t working',
  'color': 'E55934'
},
 {
   'name': '01 - type: question',
   'description': 'Further information is requested',
   'color': 'D876E3'
 },
 {
   'name': '01 - type: epic',
   'description': 'General discussion about futur and current topics',
   'color': '96D6F2'
 },
 {
   'name': '01 - type: story',
   'description':
       'Clear roadmap to implement a new feature, refactor code, etc.',
   'color': '78CBEE'
 },
 {
   'name': '01 - type: task',
   'description':
       'Something to do, e.g. fix a bug, implement a feature, add documentation, etc.',
   'color': '5BC0EB'
 },
 {
   'name': '90 - priority: high',
   'description': 'High priority subject',
   'color': 'E55934'
 },
 {
   'name': '90 - priority: low',
   'description': 'Low priority subject',
   'color': 'EE957D'
 },
 {
   'name': '90 - priority: medium',
   'description': 'Medium priority subject',
   'color': 'E97758'
 },
 {
   'name': '99 - status: backlog',
   'description': 'Task, story or epic to discuss before starting',
   'color': 'C4C4C4'
 },
 {
   'name': '99 - status: done',
   'description': 'Bug fixed or completed task, story or epic',
   'color': '9BC53D'
 },
 {
   'name': '99 - status: in progress',
   'description': 'Work in progress',
   'color': 'FDEF8D'
 },
 {
   'name': '99 - status: review approved',
   'description': 'PR has been approved',
   'color': 'FFCD03'
 },
 {
   'name': '99 - status: review needed',
   'description': 'PR needs review',
   'color': 'FFDF5B'
 },
 {
   'name': '99 - status: to do',
   'description': 'Task, story or epic ready to be tackled',
   'color': '00ccff'
 },
 {
   'name': '99 - status: won\'t fix',
   'description': 'This will not be worked on',
   'color': 'FFFFFF'
 }].forEach(function(label) {
  addLabel(label)
})

function updateLabel(label) {
  var flag = false;
  [].slice.call(document.querySelectorAll('.labels-list-item'))
      .forEach(function(element) {
        if (element.querySelector('.label-link').textContent.trim() ===
            label.name) {
          flag = true
          element.querySelector('.js-edit-label').click()
          element.querySelector('.js-new-label-name-input').value = label.name
          element.querySelector('.js-new-label-description-input').value =
              label.description
          element.querySelector('.js-new-label-color-input').value =
              '#' + label.color
          element.querySelector('.js-edit-label-cancel ~ .btn-primary').click()
        }
      })
  return flag
}

function addNewLabel(label) {
  document.querySelector('.js-new-label-name-input').value = label.name
  document.querySelector('.js-new-label-description-input').value =
      label.description
  document.querySelector('.js-new-label-color-input').value = '#' + label.color
  document.querySelector('.js-details-target ~ .btn-primary').disabled = false
  document.querySelector('.js-details-target ~ .btn-primary').click()
}

function addLabel(label) {
  if (!updateLabel(label)) addNewLabel(label)
}
