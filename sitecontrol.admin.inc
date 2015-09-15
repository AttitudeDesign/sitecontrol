<?php
//Notes: http://drupal.org/node/751826
//API: http://api.drupal.org/api/drupal/developer--topics--forms_api_reference.html/6
function sitecontrol_admin_settings() {
	$form['sitecontrol_area'] = array(
		'#type' => 'fieldset',
		'#title' => t('Type settings'),
	);
	$dest = array('destination' => $_GET['q']);
	$types = _node_types_build()->types;
	if(count($types)>0) {
		foreach($types as $type => $data) {
			if($type=='article' || $type=='page' || $type=='webform' || $type=='image') {
				$out_types1[$type] = $data->name.' ('.l('Edit »','admin/structure/types/manage/'.$type,array('query'=>$dest)).')';
				drupal_add_js('
				jQuery(document).ready(function(){
					jQuery("#edit-sitecontrol-types-'.$type.'").attr("disabled", "disabled");               
				});', 'inline');
			}
			else {
				$out_types2[$type] = $data->name.' ('.l('Edit »','admin/structure/types/manage/'.$type,array('query'=>$dest)).')';
			}
		}
		if(empty($out_types2)) {
			$out_types = $out_types1;
		}
		else {
			$out_types = array_merge($out_types1, $out_types2);
		}
		if(variable_get('sitecontrol_types', array('page', 'article'))!=array('page'=>'Page','article'=>'Article')) {
			$out_set = array_merge(array('page','article'),variable_get('sitecontrol_types', array(NULL)));
		}
		else {
			$out_set = variable_get('sitecontrol_types', array('page', 'article'));
		}
		$form['sitecontrol_area']['sitecontrol_types'] = array(
			'#type' => 'checkboxes', 
			'#title' => 'Content types for sidebar',
			'#default_value' => $out_set,
			'#options' => $out_types,
			'#description' => t('Users with the <em>administer nodes</em> permission will be able to override these options.'),
			'#suffix' => '&nbsp;&nbsp;'.t(l('Add more types »','admin/structure/types/add',array('query' => $dest)))
		);
	}
	
	
	$form['sitecontrol_volcabs'] = array(
		'#type' => 'fieldset',
		'#title' => t('Category settings'),
	);
	$vols = taxonomy_get_vocabularies();
	if(count($vols)<1) {
		$form['sitecontrol_volcabs']['sitecontrol_blogcat'] = array(
			'#type' => 'select',
			'#title' => t('Blog Vocabulary'),
			'#default_value' => variable_get('sitecontrol_blogcat','0'),
			'#disabled' => true,
			'#options' => array(
				'0' => t('Do not include in menu')
			),
			'#suffix' => t('There are no vocabularies, '.l('click here','admin/structure/taxonomy/add',array('query' => $dest)).' to create one.'),
		);
	}
	else {
		$out_vols[0] = t('Do not include in menu');
		foreach($vols as $vol) {
			$out_vols[$vol->vid] = t($vol->name);
		}
		$form['sitecontrol_volcabs']['sitecontrol_blogcat'] = array(
			'#type' => 'select',
			'#title' => t('Blog Vocabulary'),
			'#options' => $out_vols,
			'#attributes' => array('onchange' => "updatehide($(this))"),
			'#default_value' => variable_get('sitecontrol_blogcat',0),
			'#suffix' => '&nbsp;'
		);
		drupal_add_js('
		function updatehide(val) {
			var value = \'#edit-sitecontrol-sidecats-\'+$(val).val();
			//console.log(value);
			//sitecontrol_sidecats
			jQuery(".sitecontrol-sidecat-choicelist :input").removeAttr("disabled");
			jQuery(value).attr("disabled", "disabled");
		}
		', 'inline');
	
	}
	$vols = taxonomy_get_vocabularies();
	if(count($vols)>0) {
		$out_vols2 = array();
		foreach($vols as $vol) {
			if(variable_get('sitecontrol_blogcat',0)==$vol->vid) {
				drupal_add_js('
				jQuery(document).ready(function(){
					jQuery("#edit-sitecontrol-sidecats-'.$vol->vid.'").attr("disabled", "disabled");               
				});
				', 'inline');
				$blogcat = $vol->vid;
			}
			$out_vols2[$vol->vid] = t($vol->name.' ('.l('Edit » ','admin/structure/taxonomy/'.$vol->machine_name.'/edit',array('query'=>$dest)).')');
		}
		$cats = variable_get('sitecontrol_sidecats',array());
		if(isset($blogcat)) { $cats[$blogcat] = $blogcat; }
		$form['sitecontrol_volcabs']['sitecontrol_sidecats'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Sidebar categories'),
			'#default_value' => $cats,
			'#attributes' => array('class' => array('sitecontrol-sidecat-choicelist')),
			'#options' => $out_vols2,
			'#suffix' => '&nbsp;'.t(l('Add more taxonomies »','admin/structure/taxonomy/add',array('query'=>$dest))),
		);
	}
	
	$vols = taxonomy_get_vocabularies();
	if(count($vols)>0) {
		$outvols[0] = '- none -';
		foreach($vols as $vol) {
			$outvols[$vol->vid] = $vol->name;
		}
		$outTypes = array();
		$types = _node_types_build()->types; foreach($types as $type => $typeinfo) { $outTypes[$typeinfo->type] = $typeinfo->name; }
		$form['sitecontrol_grouptax'] = array(
			'#type' => 'fieldset',
			'#title' => t('Content types that relate to categories'),
			'#collapsible' => true,
			'#collapsed' => false,
			'#tree' => TRUE
		);
		$cursels = variable_get('sitecontrol_grouptax',$outTypes);
		foreach($types as $typename => $fancyname) {
			$fancyname = $fancyname->name;
			if(isset($cursels[$typename])) { $val = $cursels[$typename]; } else { $val = NULL; }
			$form['sitecontrol_grouptax'][$typename] = array(
				'#type' => 'select',
				'#title' => $fancyname.' applies to',
				'#default_value' => $val,
				'#options' => $outvols
			);
		}
	}





	$db = variable_get('sitecontrol_fieldsets',array());
	if(count($db)>0) {
		$form['sitecontrol_fieldsets'] = array(
			'#type' => 'fieldset',
			'#title' => t('Hidden Fieldsets'),
			'#collapsible' => true,
			'#collapsed' => true
		);
		foreach($db as $type => $fieldsets) {
			foreach($fieldsets as $set) {
				$sets[$set] = ucwords(str_replace('_',' ',$set));
			}
			$sitecontrol_hide = variable_get('sitecontrol_hide_'.$type,array());
			$form['sitecontrol_fieldsets']['sitecontrol_hide_'.$type] = array(
				'#type' => 'checkboxes',
				'#title' => t(ucfirst($type)),
				'#default_value' => $sitecontrol_hide,
				'#options' =>  $sets,
			);
			$sets = NULL;
		}
	}

	$form['sitecontrol_extralinks'] = array(
		'#type' => 'fieldset',
		'#title' => t('Extra links'),
		'#collapsible' => true,
		'#tree' => TRUE
	);
	$db = variable_get('sitecontrol_extralinks',array());
	if(empty($db)) {
		$form['sitecontrol_extralinks']['extra1'] = array(
			'#type' => 'fieldset',
			'#title' => t('Extra link 1'),
			'#suffix' => t('To add another link, please fill in the one above and save the page.')
		);
			$form['sitecontrol_extralinks']['extra1']['title'] = array(
				'#type' => 'textfield',
				'#title' => t('Link Text'),
				'#size' => 60,
				'#maxlength' => 128,
				'#default_value' => '',
			);
			$form['sitecontrol_extralinks']['extra1']['url'] = array(
				'#type' => 'textfield',
				'#title' => t('Link URL'),
				'#size' => 60,
				'#maxlength' => 128,
				'#default_value' => '',
			);
	}
	else {
		$next = count($db)+1; $next = 'extra'.$next;
		$x = 0;
		foreach($db as $formelement => $data) {
			if(!empty($data['title'])) {
				$x++;
				$formelement = 'extra'.$x;
				$form['sitecontrol_extralinks'][$formelement] = array(
					'#type' => 'fieldset',
					'#title' => t('Extra link '.$x)
				);
				$form['sitecontrol_extralinks'][$formelement]['title'] = array(
					'#type' => 'textfield',
					'#title' => t('Link Text'),
					'#size' => 60,
					'#maxlength' => 128,
					'#default_value' => $data['title'],	
				);
				$form['sitecontrol_extralinks'][$formelement]['url'] = array(
					'#type' => 'textfield',
					'#title' => t('Link URL'),
					'#size' => 60,
					'#maxlength' => 128,
					'#default_value' => $data['url'],	
				);
			}
		}
		$form['sitecontrol_extralinks'][$next] = array(
			'#type' => 'fieldset',
			'#title' => t('Extra link '.($x+1)),
			'#suffix' => t('To add another link, please fill in the one above and save the page.')
		);
		$form['sitecontrol_extralinks'][$next]['title'] = array(
			'#type' => 'textfield',
			'#title' => t('Link Text'),
			'#size' => 60,
			'#maxlength' => 128,
			'#default_value' => '',
		);
		$form['sitecontrol_extralinks'][$next]['url'] = array(
			'#type' => 'textfield',
			'#title' => t('Link URL'),
			'#size' => 60,
			'#maxlength' => 128,
			'#default_value' => '',
		);
	}
	
	return system_settings_form($form);
}