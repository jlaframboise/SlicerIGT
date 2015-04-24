/*==============================================================================

  Program: 3D Slicer

  Portions ( c ) Copyright Brigham and Women's Hospital ( BWH ) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerMarkupsToModelModuleWidget.h"
#include "ui_qSlicerMarkupsToModelModuleWidget.h"

#include "vtkMRMLMarkupsToModelNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkSlicerMarkupsToModelLogic.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLInteractionNode.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMarkupsToModelModuleWidgetPrivate: public Ui_qSlicerMarkupsToModelModuleWidget
{
  Q_DECLARE_PUBLIC( qSlicerMarkupsToModelModuleWidget ); 

protected:
  qSlicerMarkupsToModelModuleWidget* const q_ptr;
public:
  qSlicerMarkupsToModelModuleWidgetPrivate( qSlicerMarkupsToModelModuleWidget& object );
  vtkSlicerMarkupsToModelLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerMarkupsToModelModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMarkupsToModelModuleWidgetPrivate::qSlicerMarkupsToModelModuleWidgetPrivate(  qSlicerMarkupsToModelModuleWidget& object ) : q_ptr( &object )
{
}


//-----------------------------------------------------------------------------
vtkSlicerMarkupsToModelLogic* qSlicerMarkupsToModelModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerMarkupsToModelModuleWidget );
  return vtkSlicerMarkupsToModelLogic::SafeDownCast( q->logic() );
}


//-----------------------------------------------------------------------------
// qSlicerMarkupsToModelModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMarkupsToModelModuleWidget::qSlicerMarkupsToModelModuleWidget( QWidget* _parent )
  : Superclass( _parent )
  , d_ptr( new qSlicerMarkupsToModelModuleWidgetPrivate ( *this ) )
{
}

//-----------------------------------------------------------------------------
qSlicerMarkupsToModelModuleWidget::~qSlicerMarkupsToModelModuleWidget()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  disconnect( d->ModuleNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeChanged() ) );
  disconnect( d->ModuleNodeComboBox, SIGNAL( nodeAddedByUser( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeAddedByUser( vtkMRMLNode* ) ) );
  //disconnect( d->ModuleNodeComboBox, SIGNAL( nodeAboutToBeRemoved( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeAboutToBeRemoved( vtkMRMLNode* ) ) );

  disconnect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  //connect( d->ModelNodeComboBox, SIGNAL(nodeAddedByUser(vtkMRMLNode* )), this, SLOT(onModelNodeAddedByUser(vtkMRMLNode* ) ) );
  //connect( d->ModelNodeComboBox, SIGNAL(nodeAboutToBeRemoved(vtkMRMLNode* )), this, SLOT(onMarkupsToModelModelNodeAboutToBeRemoved(vtkMRMLNode* ) ) );

  disconnect( d->MarkupsNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCurrentMarkupsNodeChanged() ) );
  disconnect( d->MarkupsNodeComboBox, SIGNAL( nodeAboutToBeEdited( vtkMRMLNode* ) ), this, SLOT( onNodeAboutToBeEdited( vtkMRMLNode* ) ) );

  disconnect( d->UpdateOutputModelPushButton, SIGNAL( clicked() ) , this, SLOT( onUpdateOutputModelPushButton() ) );
  disconnect( d->DeleteAllPushButton, SIGNAL( clicked() ) , this, SLOT( onDeleteAllPushButton() ) );
  disconnect( d->DeleteLastPushButton, SIGNAL( clicked() ) , this, SLOT( onDeleteLastModelPushButton() ) );
  disconnect( d->PlacePushButton, SIGNAL( clicked() ), this, SLOT( onPlacePushButtonClicked() ) ); 

  disconnect( d->AutoUpdateOutputCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onAutoUpdateOutputToogled( bool ) ) );

  disconnect( d->ButterflySubdivisionCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onButterflySubdivisionToogled( bool ) ) );

  disconnect( d->CleanMarkupsCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onCleanMarkupsToogled( bool ) ) );
  disconnect( d->ClosedSurfaceRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( onModeGroupBoxClicked( bool ) ) );
  disconnect( d->CurveRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( onModeGroupBoxClicked( bool ) ) );
  disconnect( d->DelaunayAlphaDoubleSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onDelaunayAlphaDoubleChanged( double ) ) );
  disconnect( d->TubeRadiusDoubleSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onTubeRadiusDoubleChanged( double ) ) );

  disconnect( d->OutputOpacitySlider, SIGNAL( valueChanged( double ) ), this, SLOT( onOutputOpacityValueChanged( double ) ) );
  disconnect( d->OutputColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( onOutputColorChanged( QColor ) ) );
  disconnect( d->OutputVisiblityCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onOutputVisibilityToogled( bool ) ) );
  disconnect( d->OutputIntersectionVisibilityCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onOutputIntersectionVisibilityToogled( bool ) ) );
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onNodeAboutToBeEdited( vtkMRMLNode* node )
{
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( node );
  this->qvtkDisconnect(markupsToModelModuleNode, vtkMRMLMarkupsToModelNode::InputDataModifiedEvent, this, SLOT(updateFromMRMLNode()));
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onCurrentMarkupsNodeChanged()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }

  this->qvtkConnect( markupsToModelModuleNode, vtkMRMLMarkupsToModelNode::InputDataModifiedEvent, this, SLOT( updateFromMRMLNode() ) );

  vtkMRMLMarkupsFiducialNode* markupsNode = vtkMRMLMarkupsFiducialNode::SafeDownCast( d->MarkupsNodeComboBox->currentNode() );
  d->logic()->SetMarkupsNode( markupsNode, markupsToModelModuleNode );
  if( markupsToModelModuleNode->GetAutoUpdateOutput() )
  {
    d->logic()->UpdateOutputModel( markupsToModelModuleNode );
  }
  d->logic()->UpdateSelectionNode( markupsToModelModuleNode );

  this->updateWidget();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::setup()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  d->setupUi( this );
  this->Superclass::setup();

  connect( d->ModuleNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeChanged() ) );
  connect( d->ModuleNodeComboBox, SIGNAL( nodeAddedByUser( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeAddedByUser( vtkMRMLNode* ) ) );
  //connect( d->ModuleNodeComboBox, SIGNAL( nodeAboutToBeRemoved( vtkMRMLNode* ) ), this, SLOT( onMarkupsToModelModuleNodeAboutToBeRemoved( vtkMRMLNode* ) ) );

  connect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  //connect( d->ModelNodeComboBox, SIGNAL(nodeAddedByUser(vtkMRMLNode* )), this, SLOT(onModelNodeAddedByUser(vtkMRMLNode* ) ) );
  //connect( d->ModelNodeComboBox, SIGNAL(nodeAboutToBeRemoved(vtkMRMLNode* )), this, SLOT(onMarkupsToModelModelNodeAboutToBeRemoved(vtkMRMLNode* ) ) );

  connect( d->MarkupsNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onCurrentMarkupsNodeChanged() ) );
  connect( d->MarkupsNodeComboBox, SIGNAL( nodeAboutToBeEdited( vtkMRMLNode* ) ), this, SLOT( onNodeAboutToBeEdited( vtkMRMLNode* ) ) );

  connect( d->UpdateOutputModelPushButton, SIGNAL( clicked() ) , this, SLOT( onUpdateOutputModelPushButton() ) );
  connect( d->DeleteAllPushButton, SIGNAL( clicked() ) , this, SLOT( onDeleteAllPushButton() ) );
  d->DeleteAllPushButton->setIcon( QIcon( ":/Icons/MarkupsDeleteAllRows.png" ) );
  connect( d->DeleteLastPushButton, SIGNAL( clicked() ) , this, SLOT( onDeleteLastModelPushButton() ) );
  d->DeleteLastPushButton->setIcon( QIcon( ":/Icons/MarkupsDeleteLast.png" ) );
  connect( d->PlacePushButton, SIGNAL( clicked() ), this, SLOT( onPlacePushButtonClicked() ) ); 
  d->PlacePushButton->setIcon( QIcon( ":/Icons/MarkupsMouseModePlace.png" ) );

  connect( d->AutoUpdateOutputCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onAutoUpdateOutputToogled( bool ) ) );

  connect( d->ButterflySubdivisionCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onButterflySubdivisionToogled( bool ) ) );

  connect( d->CleanMarkupsCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onCleanMarkupsToogled( bool ) ) );
  connect( d->ClosedSurfaceRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( onModeGroupBoxClicked( bool ) ) );
  connect( d->CurveRadioButton, SIGNAL( toggled( bool ) ), this, SLOT( onModeGroupBoxClicked( bool ) ) );
  connect( d->DelaunayAlphaDoubleSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onDelaunayAlphaDoubleChanged( double ) ) );
  connect( d->TubeRadiusDoubleSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onTubeRadiusDoubleChanged( double ) ) );

  connect( d->OutputOpacitySlider, SIGNAL( valueChanged( double ) ), this, SLOT( onOutputOpacityValueChanged( double ) ) );
  connect( d->OutputColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( onOutputColorChanged( QColor ) ) );
  connect( d->OutputVisiblityCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onOutputVisibilityToogled( bool ) ) );
  connect( d->OutputIntersectionVisibilityCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( onOutputIntersectionVisibilityToogled( bool ) ) );
  connect( d->TextScaleSlider, SIGNAL( valueChanged( double ) ), this, SLOT( onTextScaleChanged( double ) ) );

}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::enter()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  if ( this->mrmlScene() == NULL )
  {
    qCritical() << "Invalid scene!";
    return;
  }

  // Create a module MRML node if there is none in the scene.

  vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass( 0, "vtkMRMLMarkupsToModelNode" );
  if ( node == NULL )
  {
    vtkSmartPointer< vtkMRMLMarkupsToModelNode > newNode = vtkSmartPointer< vtkMRMLMarkupsToModelNode >::New();
    this->mrmlScene()->AddNode( newNode );
  }

  node = this->mrmlScene()->GetNthNodeByClass( 0, "vtkMRMLMarkupsToModelNode" );
  if ( node == NULL )
  {
    qCritical( "Failed to create module node" );
    return;
  }

  //// For convenience, select a default module.
  if ( d->ModuleNodeComboBox->currentNode() == NULL )
  {
    d->ModuleNodeComboBox->setCurrentNodeID( node->GetID() );
  }
  onModeGroupBoxClicked( true );

  //TODO UPDATE selectionNode according to markups toolbox when markups module enter selected
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Selected node not a valid module node" );
    return;
  }
  d->logic()->UpdateSelectionNode( markupsToModelModuleNode );

  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::updateWidget()
{
  //qCritical( "HOLAS" );
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLNode* currentModuleNode = d->ModuleNodeComboBox->currentNode();
  if ( currentModuleNode == NULL )
  {
    d->MarkupsNodeComboBox->setCurrentNodeID( "" );
    d->MarkupsNodeComboBox->setEnabled( false );
    return;
  }
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( currentModuleNode );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Selected node not a valid module node" );
    return;
  }
  vtkMRMLDisplayableNode* currentMarkupsNode = vtkMRMLDisplayableNode::SafeDownCast( d->MarkupsNodeComboBox->currentNode() );
  d->DeleteAllPushButton->blockSignals( true );
  d->DeleteLastPushButton->blockSignals( true );
  d->UpdateOutputModelPushButton->blockSignals( true );
  if ( currentMarkupsNode == NULL )
  {
    d->DeleteAllPushButton->setChecked( Qt::Unchecked );
    d->DeleteLastPushButton->setChecked( Qt::Unchecked );
    d->UpdateOutputModelPushButton->setChecked( Qt::Unchecked );
    // This will ensure that we refresh the widget next time we move to a non-null widget ( since there is guaranteed to be a modified status of larger than zero )
    //return;
    d->DeleteAllPushButton->setEnabled( false );
    d->DeleteLastPushButton->setEnabled( false );
    d->UpdateOutputModelPushButton->setEnabled( false );
  }
  else
  {
    // Set the button indicating if this list is active
    if ( markupsToModelModuleNode->GetMarkupsNode()!=NULL && markupsToModelModuleNode->GetMarkupsNode()->GetNumberOfFiducials() > 0 )
    {
      d->DeleteAllPushButton->setEnabled( true );
      d->DeleteLastPushButton->setEnabled( true );
      if( markupsToModelModuleNode->GetMarkupsNode()->GetNumberOfFiducials() >= MINIMUM_MARKUPS_NUMBER )
      {
        d->UpdateOutputModelPushButton->setEnabled( true );
      }
      else
      {
        d->UpdateOutputModelPushButton->setEnabled( false );
      }
    }
    else
    {
      d->DeleteAllPushButton->setEnabled( false );
      d->DeleteLastPushButton->setEnabled( false );
      d->UpdateOutputModelPushButton->setEnabled( false );
    }
  }
  d->DeleteAllPushButton->blockSignals( false );
  d->DeleteLastPushButton->blockSignals( false );
  d->UpdateOutputModelPushButton->blockSignals( false );
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::updateFromMRMLNode()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLNode* currentNode = d->ModuleNodeComboBox->currentNode();
  if ( currentNode == NULL )
  {
    d->MarkupsNodeComboBox->setCurrentNodeID( "" );
    d->MarkupsNodeComboBox->setEnabled( false );
    return;
  }
  vtkMRMLMarkupsToModelNode* MarkupsToModelNode = vtkMRMLMarkupsToModelNode::SafeDownCast( currentNode );
  if ( MarkupsToModelNode == NULL )
  {
    qCritical( "Selected node not a valid module node" );
    return;
  }
  d->MarkupsNodeComboBox->setEnabled( true );
  
  if ( /*d->ModelNodeComboBox->currentNode() == 0 &&*/ MarkupsToModelNode->GetMarkupsNode() != NULL )
  {
    d->MarkupsNodeComboBox->setCurrentNodeID( MarkupsToModelNode->GetMarkupsNode()->GetID() );
  }
  else
  {
    d->MarkupsNodeComboBox->setCurrentNodeIndex( 0 );
  }

  switch( MarkupsToModelNode->GetModelType() )
  {
  case vtkMRMLMarkupsToModelNode::ClosedSurface: d->ClosedSurfaceRadioButton->setChecked( 1 ); break;
  case vtkMRMLMarkupsToModelNode::Curve: d->CurveRadioButton->setChecked( 1 ); break;
  }
  d->CleanMarkupsCheckBox->setChecked( MarkupsToModelNode->GetCleanMarkups() );
  d->ButterflySubdivisionCheckBox->setChecked( MarkupsToModelNode->GetButterflySubdivision() );
  d->DelaunayAlphaDoubleSpinBox->setValue( MarkupsToModelNode->GetDelaunayAlpha() );


  if ( /*d->ModelNodeComboBox->currentNode() == 0 && */MarkupsToModelNode->GetModelNode() != NULL )
  {
    d->ModelNodeComboBox->setCurrentNodeID( MarkupsToModelNode->GetModelNode()->GetID() );
    //qCritical( "Model not null" );
  }
  else
  {
    d->ModelNodeComboBox->setCurrentNodeID( "None" );
  }
  d->AutoUpdateOutputCheckBox->setChecked( MarkupsToModelNode->GetAutoUpdateOutput() );

  d->OutputOpacitySlider->setValue( MarkupsToModelNode->GetOutputOpacity() );
  double outputColor[3];
  MarkupsToModelNode->GetOutputColor( outputColor );
  QColor nodeOutputColor;
  nodeOutputColor.setRgbF( outputColor[0],outputColor[1],outputColor[2] );
  d->OutputColorPickerButton->setColor( nodeOutputColor );
  d->OutputVisiblityCheckBox->setChecked( MarkupsToModelNode->GetOutputVisibility() );
  d->OutputIntersectionVisibilityCheckBox->setChecked( MarkupsToModelNode->GetOutputIntersectionVisibility() );
  this->updateWidget();
}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onModelNodeChanged()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  
  vtkMRMLNode* currentNode = d->ModuleNodeComboBox->currentNode();
  if ( currentNode == NULL )
  {
    return;
  }
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( currentNode );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Selected node not a valid module node" );
    return;
  }

  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( d->ModelNodeComboBox->currentNode() );
  if ( modelNode == NULL )
  {
    qCritical( "Selected node not a valid model node" );
    markupsToModelModuleNode->SetModelNode( NULL );
    markupsToModelModuleNode->SetModelNodeName( "" );
    return;
  }

  markupsToModelModuleNode->SetModelNode( modelNode );
  markupsToModelModuleNode->SetModelNodeName( modelNode->GetName() );

  UpdateOutputModel();
}

////-----------------------------------------------------------------------------
//void qSlicerMarkupsToModelModuleWidget::onModelNodeAddedByUser( vtkMRMLNode* nodeAdded )
//{
//  Q_D( qSlicerMarkupsToModelModuleWidget );
//  if ( this->mrmlScene() == NULL )
//  {
//    qCritical() << "Invalid scene!";
//    return;
//  }
//  // For convenience, select a default module.
//  if( nodeAdded==NULL )
//  {
//    return;
//  }
//  vtkMRMLModelNode* ModelNodeAdded = vtkMRMLModelNode::SafeDownCast( nodeAdded );
//  if( ModelNodeAdded==NULL )
//  {
//    return;
//  }
//}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onMarkupsToModelModuleNodeChanged()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  d->logic()->UpdateSelectionNode( markupsToModelModuleNode );
  this->updateFromMRMLNode();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onMarkupsToModelModuleNodeAddedByUser( vtkMRMLNode* nodeAdded )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  if ( this->mrmlScene() == NULL )
  {
    qCritical() << "Invalid scene!";
    return;
  }

  // For convenience, select a default module.
  if( nodeAdded==NULL )
  {
    return;
  }
  vtkMRMLMarkupsToModelNode* MarkupsToModelNodeAdded = vtkMRMLMarkupsToModelNode::SafeDownCast( nodeAdded );
  if( MarkupsToModelNodeAdded==NULL )
  {
    return;
  }
  this->updateFromMRMLNode();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onUpdateOutputModelPushButton()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  d->logic()->UpdateOutputModel( markupsToModelModuleNode );
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::UpdateOutputModel()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  if( markupsToModelModuleNode->GetAutoUpdateOutput() )
  {
    d->logic()->UpdateOutputModel( markupsToModelModuleNode );
  }
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onDeleteAllPushButton()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->RemoveAllMarkups();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onModeGroupBoxClicked( bool nana )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  //qCritical( "HOLAS" );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Conversion Mode changed with no module node selection" );
    return;
  }

  if( d->ClosedSurfaceRadioButton->isChecked() )
  {
    d->ButterflySubdivisionCheckBox->setVisible( true );
    d->DelaunayAlphaLabel->setVisible( true );
    d->DelaunayAlphaDoubleSpinBox->setVisible( true );

    d->CurveInterpolationGroupBox->setVisible( false );
    d->TubeRadiusLabel->setVisible( false );
    d->TubeRadiusDoubleSpinBox->setVisible( false );
    markupsToModelModuleNode->SetModelType( vtkMRMLMarkupsToModelNode::ClosedSurface );
  }
  else
  {
    d->ButterflySubdivisionCheckBox->setVisible( false );
    d->DelaunayAlphaLabel->setVisible( false );
    d->DelaunayAlphaDoubleSpinBox->setVisible( false );

    d->CurveInterpolationGroupBox->setVisible( true );
    d->TubeRadiusLabel->setVisible( true );
    d->TubeRadiusDoubleSpinBox->setVisible( true );
    markupsToModelModuleNode->SetModelType( vtkMRMLMarkupsToModelNode::Curve );
  }
  UpdateOutputModel();
}

void qSlicerMarkupsToModelModuleWidget::onTubeRadiusDoubleChanged( double tubeRadius )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetTubeRadius( tubeRadius );
  UpdateOutputModel();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onDelaunayAlphaDoubleChanged( double delaunayAlpha )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetDelaunayAlpha( delaunayAlpha );
  UpdateOutputModel();
}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onOutputOpacityValueChanged( double outputOpacity )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetOutputOpacity( outputOpacity );
}



//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onOutputColorChanged( QColor newColor )
{
   Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetOutputColor( newColor.redF(), newColor.greenF(), newColor.blueF() );
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onOutputVisibilityToogled( bool outputVisibility )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetOutputVisibility( outputVisibility );
}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onTextScaleChanged( double textScale )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  if (d->logic()->MarkupsLogic == 0)
  {
    return;
  }
  d->logic()->MarkupsLogic->SetDefaultMarkupsDisplayNodeTextScale( textScale );
}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onOutputIntersectionVisibilityToogled( bool outputIntersectionVisibility )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetOutputIntersectionVisibility( outputIntersectionVisibility );
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onCleanMarkupsToogled( bool cleanMarkups )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetCleanMarkups( cleanMarkups );
  UpdateOutputModel();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onButterflySubdivisionToogled( bool butterflySubdivision )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetButterflySubdivision( butterflySubdivision );
  UpdateOutputModel();
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onAutoUpdateOutputToogled( bool autoUpdateOutput )
{
  Q_D( qSlicerMarkupsToModelModuleWidget );
  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->SetAutoUpdateOutput( autoUpdateOutput );
}


//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onDeleteLastModelPushButton()
{
  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsToModelNode* markupsToModelModuleNode = vtkMRMLMarkupsToModelNode::SafeDownCast( d->ModuleNodeComboBox->currentNode() );
  if ( markupsToModelModuleNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  markupsToModelModuleNode->RemoveLastMarkup();

}

//-----------------------------------------------------------------------------
void qSlicerMarkupsToModelModuleWidget::onPlacePushButtonClicked()
{

  Q_D( qSlicerMarkupsToModelModuleWidget );

  vtkMRMLMarkupsNode* currentMarkupsNode = vtkMRMLMarkupsNode::SafeDownCast( d->MarkupsNodeComboBox->currentNode() );

  // Depending to the current state, change the activeness and placeness for the current markups node
  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast( this->mrmlScene()->GetNodeByID( "vtkMRMLInteractionNodeSingleton" ) );

  bool isActive = currentMarkupsNode != NULL && d->logic()->MarkupsLogic->GetActiveListID().compare( currentMarkupsNode->GetID() ) == 0;
  bool isPlace = interactionNode->GetCurrentInteractionMode() == vtkMRMLInteractionNode::Place;

  if ( isPlace && isActive )
  {
    interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::ViewTransform );
  }
  else
  {
    interactionNode->SetCurrentInteractionMode( vtkMRMLInteractionNode::Place );
    // interactionNode->SetPlaceModePersistence( true ); // Use whatever persistence the user has already set
  }

  if ( currentMarkupsNode != NULL )
  {
    d->logic()->MarkupsLogic->SetActiveListID( currentMarkupsNode ); // If there are other widgets, they are responsible for updating themselves
  }

  this->updateWidget();
}


